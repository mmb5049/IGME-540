#include "ShaderStruct.hlsli"
Texture2D SurfaceTexture : register(t0);
SamplerState BasicSampler : register(s0);


cbuffer externalData : register(b0)
{
    
    float4 colorTint;
    float totalTime;
    float3 padding;
    float2 UVScale;
    float2 UVOffset;
    float3 cameraPosition;
    float cameraPadding;

    float3 ambientColor;
    float ambientPadding;

    Light lights[6];
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
    float3 baseColor = surfaceColor.rgb * colorTint.rgb;

    // Ambient lighting
    float3 totalLight =
        ambientColor * baseColor;

    // Directional diffuse lighting
    float3 totalDiffuseAndSpecular = float3(0, 0, 0);

    for (int i = 0; i < 6; i++)
    {

        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            totalDiffuseAndSpecular += CalculateDirectionalLight(
                lights[i],
                input.normal,
                baseColor,
                input.worldPosition,
                cameraPosition
            );
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            totalDiffuseAndSpecular += CalculatePointLight(
            lights[i],
            input.normal,
            baseColor,
            input.worldPosition,
            cameraPosition
        );
        }
        else if (lights[i].Type == LIGHT_TYPE_SPOT)
        {
            totalDiffuseAndSpecular += CalculateSpotLight(
            lights[i],
            input.normal,
            baseColor,
            input.worldPosition,
            cameraPosition
        );
        }
    }

    totalLight += totalDiffuseAndSpecular;

    return float4(totalLight, 1);
}