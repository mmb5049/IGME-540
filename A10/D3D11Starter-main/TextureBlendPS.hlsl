

Texture2D WoodTexture : register(t0);
Texture2D BrickTexture : register(t1);
SamplerState BasicSampler : register(s0);

cbuffer externalData : register(b0)
{
    float4 ColorTint;
    float TotalTime;
    float3 Padding;

    float2 UVScale;
    float2 UVOffset;
};


struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

float4 main(VertexToPixel input) : SV_TARGET
{
    float2 uv = input.uv * UVScale + UVOffset;

    float4 woodColor =
        WoodTexture.Sample(BasicSampler, uv);

    float4 brickColor =
        BrickTexture.Sample(BasicSampler, uv);

    // Multiply the textures together.
    float4 surfaceColor = woodColor * brickColor;

    return surfaceColor * ColorTint;
}
