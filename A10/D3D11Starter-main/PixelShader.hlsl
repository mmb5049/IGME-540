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
   // Apply UV transformations
    float2 uv = input.uv * UVScale + UVOffset;



// Sample normal map
    float3 normalSample = NormalMap.Sample(
    BasicSampler, uv
).rgb;

// Unpack normal from [0, 1] to [-1, 1]
    float3 tangentNormal = normalize(
    normalSample * 2.0f - 1.0f
    );
    
    
    

// Get interpolated normal and tangent
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);

// Make tangent perpendicular to normal
    T = normalize(T - N * dot(T, N));

// Calculate bitangent
    float3 B = normalize(cross(N, T));

// Create TBN matrix
    float3x3 TBN = float3x3(T, B, N);

// Transform tangent-space normal into world space
    float3 normal = normalize(mul(tangentNormal, TBN));
    
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    surfaceColor *= colorTint.rgb;

// Ambient lighting
    float3 totalLight = ambientColor * surfaceColor;

    // Directional diffuse lighting

    for (int i = 0; i < 6; i++)
    {
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            totalLight += CalculateDirectionalLight(
            lights[i],
            normal,
            surfaceColor,
            input.worldPosition,
            cameraPosition
        );
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            totalLight += CalculatePointLight(
            lights[i],
            normal,
            surfaceColor,
            input.worldPosition,
            cameraPosition
        );
        }
        else if (lights[i].Type == LIGHT_TYPE_SPOT)
        {
            totalLight += CalculateSpotLight(
            lights[i],
            normal,
            surfaceColor,
            input.worldPosition,
            cameraPosition
        );
        }
    }

    return float4(totalLight, 1.0f);
    
}