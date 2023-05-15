float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gViewInverse : ViewInverse;
Texture2D gParticleTexture;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

//STATES
//******
BlendState AlphaBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

DepthStencilState DisableDepthWriting
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};


//SHADER STRUCTS
//**************
struct VS_DATA
{
	float3 Position : POSITION;
	float4 Color: COLOR;
	float Size : TEXCOORD0;
	float Rotation : TEXCOORD1;
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float2 TexCoord: TEXCOORD0;
	float4 Color : COLOR;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float2 texCoord, float4 col, float2x2 uvRotation)
{
	GS_DATA output = (GS_DATA)0;

	output.Position = mul(float4(pos, 1.0f), gWorldViewProj);
	output.TexCoord = texCoord;
	output.Color = col;

	triStream.Append(output);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	float3 topLeft, topRight, bottomLeft, bottomRight;
	float size = vertex[0].Size;
	float3 origin = vertex[0].Position;

	topLeft = float3(-size * 0.5f, size * 0.5f, 0.0f);
	topLeft = mul(topLeft, (float3x3) gViewInverse) + origin;
	topRight = float3(size * 0.5f, size * 0.5f, 0.0f);
	topRight = mul(topRight, (float3x3) gViewInverse) + origin;
	bottomLeft = float3(-size * 0.5f, -size * 0.5f, 0.0f);
	bottomLeft = mul(bottomLeft, (float3x3) gViewInverse) + origin;
	bottomRight = float3(size * 0.5f, -size * 0.5f, 0.0f);
	bottomRight = mul(bottomRight, (float3x3) gViewInverse) + origin;

	float2x2 uvRotation = { cos(vertex[0].Rotation), -sin(vertex[0].Rotation), sin(vertex[0].Rotation), cos(vertex[0].Rotation) };

	CreateVertex(triStream, bottomLeft, float2(0, 1), vertex[0].Color, uvRotation);
	CreateVertex(triStream, topLeft, float2(0, 0), vertex[0].Color, uvRotation);
	CreateVertex(triStream, bottomRight, float2(1, 1), vertex[0].Color, uvRotation);
	CreateVertex(triStream, topRight, float2(1, 0), vertex[0].Color, uvRotation);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{
	float4 result = gParticleTexture.Sample(samPoint,input.TexCoord);
	return input.Color * result;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));

		SetRasterizerState(BackCulling);
		SetDepthStencilState(DisableDepthWriting, 0);
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}
