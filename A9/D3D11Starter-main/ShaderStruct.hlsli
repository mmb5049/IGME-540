#ifndef __SHADERSTUCTS__
#define __SHADERSTUCTS__

// Shared structure between vertex and pixel shaders
struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;
    float2 uv               : TEXCOORD;
    float3 normal           : NORMAL;
    float3 worldPosition    : POSITION;

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


float3 CalculateDirectionalLight(
    Light light,
    float3 normal,
    float3 surfaceColor,
    float3 worldPosition,
    float3 cameraPosition)
{
    // Direction from surface toward the light
    float3 lightDirection = normalize(-light.Direction);

    // Diffuse
    float diffuseAmount = max(dot(normal, lightDirection), 0.0f);

    float3 diffuse =
        surfaceColor * light.Color * light.Intensity * diffuseAmount;

    // Specular
    float3 viewDirection = normalize(cameraPosition - worldPosition);

    // reflect() expects the direction the light is traveling
    float3 reflectionDirection = reflect(-lightDirection, normal);

    float specularAmount = 0.0f;

    if (diffuseAmount > 0.0f)
    {
        specularAmount = pow(
            max(dot(reflectionDirection, viewDirection), 0.0f),
            128.0f
        );
    }

    float3 specular =
        light.Color * light.Intensity * specularAmount;

    return diffuse + specular;
}

#endif