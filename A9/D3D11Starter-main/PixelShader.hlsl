Texture2D SurfaceTexture : register(t0);
SamplerState BasicSampler : register(s0);


cbuffer externalData : register(b0)
{
    float3 colorTint;
    float totalTime;
    float3 padding;
    float2 UVScale;
    float2 UVOffset;
};


struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;
    float2 uv               : TEXCOORD;
    float3 normal           : NORMAL;

};


float4 main(VertexToPixel input) : SV_TARGET
{
    float2 uv = input.uv * UVScale + UVOffset;

    float4 surfaceColor =
        SurfaceTexture.Sample(BasicSampler, uv);


    return surfaceColor * float4(colorTint, 0.0f);

}