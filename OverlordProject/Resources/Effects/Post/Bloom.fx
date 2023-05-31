//=============================================================================
//// Shader uses position and texture
//=============================================================================
//// WIP: additional tweaking & parametrization needed
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

// IN/OUT STRUCTS
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

// Vertex shader
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position, 1.0f);
    output.TexCoord = input.TexCoord;
    return output;
}

// Pixel shader
float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = gTexture.Sample(samPoint, input.TexCoord);

    // Apply horizontal Gaussian blur
    float4 blurColorH = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = -5; i <= 5; i++)
    {
        float2 offset = float2(i, 0.0f) * 0.001f;
        blurColorH += gTexture.Sample(samPoint, input.TexCoord + offset);
    }
    blurColorH /= 30.0f;

    // Apply vertical Gaussian blur
    float4 blurColorV = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = -5; i <= 5; i++)
    {
        float2 offset = float2(0.0f, i) * 0.001f;
        blurColorV += gTexture.Sample(samPoint, input.TexCoord + offset);
    }
    blurColorV /= 30.0f;

    float4 result = color + blurColorH + blurColorV;

    return result;
}

// Technique
technique11 Bloom
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}
