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


float3 CalculateDirectionalLight(
    Light light,
    float3 normal, // Assumed to be normalized in the pixel shader calling this
    float3 surfaceColor,
    float3 worldPosition,
    float3 cameraPosition)
{
    float3 lightDirection = normalize(-light.Direction);
    float3 viewDirection = normalize(cameraPosition - worldPosition);

    // Diffuse
    float diffuseAmount = saturate(dot(normal, lightDirection));

    // Specular
    float3 reflectionDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 256.0f);
    
    // Mask specular so it doesn't show up on the dark side of the object
    specularAmount *= (diffuseAmount > 0.0f);

    // Combine: Surface color only affects diffuse; Light color/intensity affects both
    float3 diffuseFinal = diffuseAmount * surfaceColor;
    float3 specularFinal = specularAmount * 1.0f; // Multiplied by 1.0f assuming a white specular highlight

    return (diffuseFinal + specularFinal) * light.Intensity * light.Color;
}

float3 CalculatePointLight(
    Light light,
    float3 normal,
    float3 surfaceColor,
    float3 worldPosition,
    float3 cameraPosition)
{
    float3 lightDirection = normalize(
        light.Position - worldPosition
    );

    // Diffuse
    float diffuseAmount = saturate(
        dot(normal, lightDirection)
    );

    float3 diffuse =
        surfaceColor * light.Color * diffuseAmount;

    // Specular
    float3 viewDirection = normalize(
        cameraPosition - worldPosition
    );

    float3 reflectionDirection = reflect(
        -lightDirection,
        normal
    );

    float specularAmount = 0.0f;

    if (diffuseAmount > 0.0f)
    {
        specularAmount = pow(
            max(dot(viewDirection, reflectionDirection), 0.0f),
            256.0f
        );
    }

    float3 specular = surfaceColor * specularAmount;

    // Attenuation
    float attenuation = Attenuate(light, worldPosition);

    return (
        surfaceColor * diffuseAmount +
        surfaceColor * specularAmount
    ) * light.Color * light.Intensity * attenuation;
}

float3 CalculateSpotLight(
    Light light,
    float3 normal,
    float3 surfaceColor,
    float3 worldPosition,
    float3 cameraPosition)
{
    // Direction from light toward pixel
    float3 fromLightToPixel = normalize(
        worldPosition - light.Position
    );

    // Calculate how aligned the pixel is with the spotlight cone
    float theta = dot(
        fromLightToPixel,
        normalize(light.Direction)
    );

    // Convert cone angles into cosine thresholds
    float innerCos = cos(light.SpotInnerAngle);
    float outerCos = cos(light.SpotOuterAngle);

    // Spotlight attenuation
    float spotAttenuation = smoothstep(
        outerCos,
        innerCos,
        theta
    );

    // Calculate regular point light contribution
    float3 pointContribution = CalculatePointLight(
        light,
        normal,
        surfaceColor,
        worldPosition,
        cameraPosition
    );

    return pointContribution * spotAttenuation;
}

#endif