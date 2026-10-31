#include "ShaderStruct.hlsli"
Texture2D SurfaceTexture : register(t0);
SamplerState BasicSampler : register(s0);


cbuffer externalData : register(b0)
{
    
    float3 colorTint;
    float totalTime;
    float3 padding;
    float2 UVScale;
    float2 UVOffset;
    float3 cameraPosition;
    float cameraPadding;

    float3 ambientColor;
    float ambientPadding;

    Light lights[MAX_LIGHTS];
}



float4 main(VertexToPixel input) : SV_TARGET
{
    // Normalize interpolated normal
    input.normal = normalize(input.normal);

    // Apply UV transformations
    float2 uv = input.uv * UVScale + UVOffset;

    // Sample texture
    float4 surfaceColor =
        SurfaceTexture.Sample(BasicSampler, uv);

    // Calculate material surface color
    float3 baseColor = surfaceColor.rgb * colorTint;

    // Ambient lighting
    float3 ambient =
        ambientColor * baseColor;

    // Directional diffuse lighting
    float3 totalDiffuseAndSpecular = float3(0, 0, 0);

    for (int i = 0; i < 5; i++)
    {
        totalDiffuseAndSpecular += CalculateDirectionalLight(
        lights[i],
        input.normal,
        baseColor,
        input.worldPosition,
        cameraPosition
        );
    }

    float3 totalLight = ambient + totalDiffuseAndSpecular;

    return float4(totalLight, surfaceColor.a);
}