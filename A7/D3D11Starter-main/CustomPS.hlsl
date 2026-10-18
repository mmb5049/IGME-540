cbuffer externalData : register(b1)
{
    float4 colorTint;
    float totalTime;
    float3 padding;
};

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

float4 main(VertexToPixel input) : SV_TARGET
{
    float2 uv = input.uv;

    // Two animated waves at different angles
    float wave1 = sin((uv.x + uv.y) * 24.0f - totalTime * 3.0f);
    float wave2 = cos((uv.x - uv.y) * 18.0f + totalTime * 2.0f);

    // Combine the waves into a repeating pattern
    float pattern = (wave1 + wave2) * 0.25f + 0.5f;

    // Blend between a dark color and the material tint
    float3 darkColor = colorTint.rgb * 0.15f;
    float3 brightColor = colorTint.rgb;

    float3 finalColor = lerp(darkColor, brightColor, pattern);

    return float4(finalColor, colorTint.a);
}