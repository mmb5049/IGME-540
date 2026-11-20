#ifndef __SHADERSTUCTS__
#define __SHADERSTUCTS__

// Shared structure between vertex and pixel shaders
struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;
    float2 uv               : TEXCOORD;
    float3 normal           : NORMAL;
    float3 tangent			: TANGENT;
    float3 worldPosition    : POSITION;

};

struct VertexShaderInput
{
    float3 localPosition    : POSITION;
    float2 uv               : TEXCOORD;
    float3 normal           : NORMAL;
    float3 tangent          : TANGENT;
};


#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2
#define MAX_LIGHTS				32

struct Light
{
    int Type;

    float3 Direction;
    float Range;

    float3 Position;
    float Intensity;

    float3 Color;

    float SpotInnerAngle;
    float SpotOuterAngle;

    float2 Padding;
};


float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);

    float att = saturate(
        1.0f - (dist * dist / (light.Range * light.Range))
    );

    return att * att;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH = max(dot(N, H), 0.0f);

    float NdotH2 = NdotH * NdotH;

    float denominator =
        NdotH2 * (a2 - 1.0f) + 1.0f;

    denominator = 3.14159265f * denominator * denominator;

    return a2 / max(denominator, 0.0001f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;

    return NdotV / (NdotV * (1.0f - k) + k);
}

float GeometricShadowing(
    float3 N,
    float3 V,
    float3 L,
    float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);

    float ggxV = GeometrySchlickGGX(NdotV, roughness);
    float ggxL = GeometrySchlickGGX(NdotL, roughness);

    return ggxV * ggxL;
}

float3 MicrofacetBRDF(
    float3 N,
    float3 L,
    float3 V,
    float roughness,
    float3 F0)
{
    float3 H = normalize(L + V);

    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float NdotH = max(dot(N, H), 0.0f);
    float VdotH = max(dot(V, H), 0.0f);

    float D = DistributionGGX(N, H, roughness);
    float G = GeometricShadowing(N, V, L, roughness);
    float3 F = FresnelSchlick(VdotH, F0);

    float denominator =
        4.0f * NdotV * NdotL + 0.0001f;

    return (D * G * F) / denominator;
}

float3 DiffuseEnergyConserve(
    float diffuse,
    float3 F,
    float metalness)
{
    return diffuse * (1.0f - F) * (1.0f - metalness);
}



float3 CalculateDirectionalLight(
    Light light,
    float3 normal,
    float3 albedo,
    float roughness,
    float metalness,
    float3 worldPosition,
    float3 cameraPosition)
{
    float3 N = normalize(normal);
    float3 L = normalize(-light.Direction);
    float3 V = normalize(cameraPosition - worldPosition);

    float NdotL = saturate(dot(N, L));

    if (NdotL <= 0.0f)
        return float3(0.0f, 0.0f, 0.0f);

    // Specular color for this material
    float3 F0 = lerp(
        float3(0.04f, 0.04f, 0.04f),
        albedo,
        metalness
    );

    // Fresnel
    float3 H = normalize(L + V);
    float3 F = FresnelSchlick(
        max(dot(H, V), 0.0f),
        F0
    );

    // Diffuse
    float diffuse = NdotL;

    float3 balancedDiff = DiffuseEnergyConserve(
        diffuse,
        F,
        metalness
    );

    // Specular
    float3 specular = MicrofacetBRDF(
        N,
        L,
        V,
        roughness,
        F0
    );

    // Combine
    return (balancedDiff * albedo + specular * NdotL)
        * light.Intensity
        * light.Color;
}

float3 CalculatePointLight(
    Light light,
    float3 normal,
    float3 albedo,
    float roughness,
    float metalness,
    float3 worldPosition,
    float3 cameraPosition)
{
    float3 N = normalize(normal);
    float3 L = normalize(light.Position - worldPosition);
    float3 V = normalize(cameraPosition - worldPosition);

    float NdotL = saturate(dot(N, L));

    if (NdotL <= 0.0f)
        return float3(0.0f, 0.0f, 0.0f);

    // Specular color
    float3 F0 = lerp(
        float3(0.04f, 0.04f, 0.04f),
        albedo,
        metalness
    );

    // Fresnel
    float3 H = normalize(L + V);
    float3 F = FresnelSchlick(
        max(dot(H, V), 0.0f),
        F0
    );

    // Diffuse with energy conservation
    float diffuse = NdotL;

    float3 balancedDiff = DiffuseEnergyConserve(
        diffuse,
        F,
        metalness
    );

    // Specular
    float3 specular = MicrofacetBRDF(
        N,
        L,
        V,
        roughness,
        F0
    );

    // Distance attenuation
    float attenuation = Attenuate(light, worldPosition);

    return (balancedDiff * albedo + specular * NdotL)
        * light.Color
        * light.Intensity
        * attenuation;
}

float3 CalculateSpotLight(
    Light light,
    float3 normal,
    float3 albedo,
    float roughness,
    float metalness,
    float3 worldPosition,
    float3 cameraPosition)
{
    // Direction from light to pixel
    float3 fromLightToPixel = normalize(
        worldPosition - light.Position
    );

    // Angle between spotlight direction and pixel
    float theta = dot(
        fromLightToPixel,
        normalize(light.Direction)
    );

    // Cone thresholds
    float innerCos = cos(light.SpotInnerAngle);
    float outerCos = cos(light.SpotOuterAngle);

    // Spotlight attenuation
    float spotAttenuation = smoothstep(
        outerCos,
        innerCos,
        theta
    );

    // Calculate PBR point light contribution
    float3 pointContribution = CalculatePointLight(
        light,
        normal,
        albedo,
        roughness,
        metalness,
        worldPosition,
        cameraPosition
    );

    return pointContribution * spotAttenuation;
}


#endif