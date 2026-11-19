
#include "ShaderStruct.hlsli"

Texture2D AlbedoTexture : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);

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
    // UV transformations
    float2 uv = input.uv * UVScale + UVOffset;

    // Albedo - convert from gamma space to linear space
    float3 albedo = pow(
        AlbedoTexture.Sample(BasicSampler, uv).rgb,
        2.2f
    );

    albedo *= colorTint.rgb;

    // Normal map - no gamma correction
    float3 normalSample = NormalMap.Sample(
        BasicSampler, uv
    ).rgb;

    float3 tangentNormal = normalize(
        normalSample * 2.0f - 1.0f
    );

    // TBN matrix
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);

    T = normalize(T - N * dot(T, N));

    float3 B = normalize(cross(N, T));

    float3x3 TBN = float3x3(T, B, N);

    float3 normal = normalize(mul(tangentNormal, TBN));

    // Roughness - no gamma correction
    float roughness = RoughnessMap.Sample(
        BasicSampler, uv
    ).r;

    // Prevent zero roughness from causing unstable calculations
    roughness = max(roughness, 0.04f);

    // Metalness - no gamma correction
    float metalness = MetalnessMap.Sample(
        BasicSampler, uv
    ).r;

    // Ambient lighting - reduced by metalness
    float3 totalLight =
        ambientColor * albedo * (1.0f - metalness);

    // Calculate direct lighting
    for (int i = 0; i < 6; i++)
    {
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            totalLight += CalculateDirectionalLight(
                lights[i],
                normal,
                albedo,
                roughness,
                metalness,
                input.worldPosition,
                cameraPosition
            );
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            totalLight += CalculatePointLight(
                lights[i],
                normal,
                albedo,
                roughness,
                metalness,
                input.worldPosition,
                cameraPosition
            );
        }
        else if (lights[i].Type == LIGHT_TYPE_SPOT)
        {
            totalLight += CalculateSpotLight(
                lights[i],
                normal,
                albedo,
                roughness,
                metalness,
                input.worldPosition,
                cameraPosition
            );
        }
    }

    // Gamma correction
    return float4(
        pow(max(totalLight, 0.0f), 1.0f / 2.2f),
        1.0f
    );
}
