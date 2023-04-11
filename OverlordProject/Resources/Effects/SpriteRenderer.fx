float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
    DepthEnable = FALSE;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
    uint TextureId : TEXCOORD0; //Can be ignored
    float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
    float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
    float4 Color : COLOR;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
    return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
    if (rotation != 0)
    {
        float2 posrg = pos.rg;
        posrg -= float2(offset.r + pivotOffset.r, offset.g + pivotOffset.g);

        float x = (posrg.x * rotCosSin.x - posrg.y * rotCosSin.y);
        float y = (posrg.y * rotCosSin.x + posrg.x * rotCosSin.y);
        pos.rg = float2(x, y) + float2(offset.r, offset.g);
    }
    else
    {
        pos.r -= pivotOffset.r;
        pos.g -= pivotOffset.g;
    }

	//Geometry Vertex Output
    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
    //Given Data (Vertex Data)
    float3 position = vertex[0].TransformData.rgb;
    float2 offset = vertex[0].TransformData.rg;   
    float rotation = vertex[0].TransformData.a;   
    float2 pivot = vertex[0].TransformData2.rg;   
    float2 scale = vertex[0].TransformData2.ba;   
    float2 texCoord = float2(0, 0);               


    // LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
    // |          / |
    // |       /    |
    // |    /       |
    // | /          |
    // LB----------RB

    float2 calcRot = float2(cos(rotation), sin(rotation));

    //VERTEX 1 [LT]
    float3 vertexPos = float3(position.r, position.g, position.b);
    CreateVertex(triStream, vertexPos, vertex[0].Color, float2(0, 0), rotation, calcRot, offset, float2(pivot.r * gTextureSize.r * scale.r, pivot.g * gTextureSize.g * scale.g));

    //VERTEX 2 [RT]
    vertexPos = float3(position.r + gTextureSize.r * scale.r, position.g, position.b);
    CreateVertex(triStream, vertexPos, vertex[0].Color, float2(1, 0), rotation, calcRot, offset, float2(pivot.r * gTextureSize.r * scale.r, pivot.g * gTextureSize.g * scale.g));

    //VERTEX 3 [LB]
    vertexPos = float3(position.r, position.g + gTextureSize.g * scale.g, position.b);
    CreateVertex(triStream, vertexPos, vertex[0].Color, float2(0, 1), rotation, calcRot, offset, float2(pivot.r * gTextureSize.r * scale.r, pivot.g * gTextureSize.g * scale.g));

    //VERTEX 4 [RB]
    vertexPos = float3(position.r + gTextureSize.r * scale.r, position.g + gTextureSize.g * scale.g, position.b);
    CreateVertex(triStream, vertexPos, vertex[0].Color, float2(1, 1), rotation, calcRot, offset, float2(pivot.r * gTextureSize.r * scale.r, pivot.g * gTextureSize.g * scale.g));

}


//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;
}

// Default Technique
technique11 Default
{
    pass p0
    {
        SetRasterizerState(BackCulling);
        SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetDepthStencilState(NoDepth,0);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, MainGS()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}
