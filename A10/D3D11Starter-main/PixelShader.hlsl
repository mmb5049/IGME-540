#include "ShaderStruct.hlsli"
Texture2D SurfaceTexture : register(t0);
Texture2D NormalMap : register(t1);
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
    // Sample the normal map
    float3 normalSample = NormalMap.Sample(BasicSampler, input.uv).rgb;

    // Unpack from [0, 1] to [-1, 1]
    float3 tangentNormal = normalSample * 2.0f - 1.0f;

    // Normalize the sampled normal
    tangentNormal = normalize(tangentNormal);

    // Get the interpolated normal and tangent
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);

    // Make sure tangent is perpendicular to normal
    T = normalize(T - N * dot(N, T));

    // Calculate bitangent
    float3 B = normalize(cross(N, T));

    // Create TBN matrix
    float3x3 TBN = float3x3(T, B, N);

    // Transform tangent-space normal into world space
    float3 normal = normalize(mul(TBN, tangentNormal));

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