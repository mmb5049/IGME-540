#ifndef __GGP_LIGHTING__
#define __GGP_LIGHTING__

// Basic lighting functions, constants and other useful structures

#define MAX_LIGHTS 32 // Make sure this matches C++!

#define DEFAULT_SPECULAR 256.0f

#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

struct Light
{
    int Type;
    float3 Direction; // 16 bytes

    float Range;
    float3 Position; // 32 bytes

    float Intensity;
    float3 Color; // 48 bytes

    float SpotInnerAngle;
    float SpotOuterAngle;
    float2 Padding; // 64 bytes
};

#endif