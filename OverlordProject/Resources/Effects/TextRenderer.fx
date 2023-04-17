float4x4 gTransform : WORLDVIEWPROJECTION;
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

RasterizerState BackCulling
{
	CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	int Channel : TEXCOORD2; //Texture Channel
	float3 Position : POSITION; //Left-Top Character Quad Starting Position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Left-Top Character Texture Coordinate on Texture
	float2 CharSize: TEXCOORD1; //Size of the character (in screenspace)
};

struct GS_DATA
{
	float4 Position : SV_POSITION; //HOMOGENEOUS clipping space position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Texcoord of the vertex
	int Channel : TEXCOORD1; //Channel of the vertex
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, int channel)
{
	GS_DATA data = (GS_DATA)0;

	data.Position = mul(float4(pos, 1.0f), gTransform);
	data.Color = col;
	data.TexCoord = texCoord;
	data.Channel = channel;

	triStream.Append(data);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	float3 pos = vertex[0].Position;
	float2 texCoord = vertex[0].TexCoord;
	float width = vertex[0].CharSize.x;
	float height = vertex[0].CharSize.y;
	float widthUV = (vertex[0].CharSize.x / gTextureSize.x);
	float heightUV = (vertex[0].CharSize.y / gTextureSize.y);

	//Left-Top
	CreateVertex(triStream, pos, vertex[0].Color, texCoord, vertex[0].Channel);
	//Right-Top
	pos.x = vertex[0].Position.x + width;
	pos.y = vertex[0].Position.y;
	texCoord.x = vertex[0].TexCoord.x + widthUV;
	texCoord.y = vertex[0].TexCoord.y;
	CreateVertex(triStream, pos, vertex[0].Color, texCoord, vertex[0].Channel);
	//Left-Bottom
	pos.x = vertex[0].Position.x;
	pos.y = vertex[0].Position.y + height;
	texCoord.x = vertex[0].TexCoord.x;
	texCoord.y = vertex[0].TexCoord.y + heightUV;
	CreateVertex(triStream, pos, vertex[0].Color, texCoord, vertex[0].Channel);
	//Right-Bottom
	pos.x = vertex[0].Position.x + width;
	pos.y = vertex[0].Position.y + height;
	texCoord.x = vertex[0].TexCoord.x + widthUV;
	texCoord.y = vertex[0].TexCoord.y + heightUV;
	CreateVertex(triStream, pos, vertex[0].Color, texCoord, vertex[0].Channel);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{
	return gSpriteTexture.Sample(samPoint, input.TexCoord)[input.Channel] * input.Color;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
