//************
// VARIABLES *
//************
float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
float4x4 m_MatrixWorld : WORLD;
float3 m_LightDirection : DIRECTION
<
    string Object = "TargetLight";
> = float3(0.577f, -0.577f, 0.577f);

float3 gColorDiffuse : COLOR = float3(1, 1, 1);

float gSpikeLength
<
    string UIWidget = "Slider";
    float UIMin = 0.0f;
    float UIMax = 0.5f;
    float UIStep = 0.0001f;
> = 0.2f;

RasterizerState FrontCulling
{
    CullMode = FRONT;
};

//**********
// STRUCTS *
//**********
struct VS_DATA
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
};

//****************
// VERTEX SHADER *
//****************
VS_DATA MainVS(VS_DATA vsData)
{
    return vsData;
}

//******************
// GEOMETRY SHADER *
//******************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float3 normal, float2 texCoord)
{
    GS_DATA gsData = (GS_DATA)0;
    gsData.Position = mul(float4(pos, 1.0f), m_MatrixWorldViewProj);
    gsData.Normal = mul(normal, (float3x3)m_MatrixWorld);
    triStream.Append(gsData);
}

[maxvertexcount(9)]
void SpikeGenerator(triangle VS_DATA vertices[3], inout TriangleStream<GS_DATA> triStream)
{
    float3 basePoint, top, left, right, spikeNormal;

    left = vertices[0].Position.xyz;
    top = vertices[1].Position.xyz;
    right = vertices[2].Position.xyz;
    basePoint = (left + right + top) / 3;

    float3 e0 = top - left;
    float3 e1 = right - left;
    spikeNormal = normalize(cross(e1, e0));

    basePoint += spikeNormal * gSpikeLength;

    //FACE 1
    e0 = basePoint - top;
    e1 = top - left;
    float3 faceNormal = normalize(cross(e0, e1));
    CreateVertex(triStream, top, faceNormal, (float2)0);
    CreateVertex(triStream, basePoint, faceNormal, (float2)0);
    CreateVertex(triStream, left, faceNormal, (float2)0);

    triStream.RestartStrip();

    //FACE 2
    e0 = basePoint - right;
    e1 = top - right;
    faceNormal = normalize(cross(e1, e0));
    CreateVertex(triStream, right, faceNormal, (float2)0);
    CreateVertex(triStream, basePoint, faceNormal, (float2)0);
    CreateVertex(triStream, top, faceNormal, (float2)0);

    triStream.RestartStrip();

    //Face 3
    e0 = basePoint - left;
    e1 = right - left;
    faceNormal = normalize(cross(e1, e0));
    CreateVertex(triStream, left, faceNormal, (float2)0);
    CreateVertex(triStream, basePoint, faceNormal, (float2)0);
    CreateVertex(triStream, right, faceNormal, (float2)0);

}

//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    input.Normal = normalize(input.Normal);
    float diffuseStrength = max(dot(normalize(m_LightDirection),-input.Normal),0.2f);
    return float4(gColorDiffuse * diffuseStrength,1.0f);
}

//*************
// TECHNIQUES *
//*************
technique11 Default //FXComposer >> Rename to "technique10 Default"
{
    pass p0
    {
        SetRasterizerState(FrontCulling);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, SpikeGenerator()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}