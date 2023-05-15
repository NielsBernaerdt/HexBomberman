//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

DepthStencilState enableDepth
{
	DepthEnable = TRUE;
};
RasterizerState BackCulling
{
	CullMode = BACK;
};


//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Position = float4(input.Position, 1);
	output.TexCoord = input.TexCoord;

	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	int mip = 0;
	int width, height, levels;

	gTexture.GetDimensions(mip, width, height,levels);
	float dx = 1.0f / width;
	float dy = 1.0f / height;

	float4 color = float4(0.f, 0.f, 0.f, 0.f);
	int nrIterations = 5;
	for (int i = 0; i < nrIterations; ++i)
	{
		for (int j = 0; j < nrIterations; ++j)
		{
			float2 offset = float2((dx * 2 * i) - dx, (dy * 2 * j) - dy);
			color += gTexture.Sample(samPoint, input.TexCoord + offset);
		}
	}
	color /= nrIterations * nrIterations;

	return color;
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}