Texture2D SurfaceTexture : register(t0);
SamplerState BasicSampler : register(s0);


cbuffer externalData : register(b0)
{
    float3 colorTint;
    float totalTime;
    float3 padding;
};


struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;
    float2 uv               : TEXCOORD;
    float3 normal           : NORMAL;

};


float4 main(VertexToPixel input) : SV_TARGET
{
    float4 surfaceColor =
        SurfaceTexture.Sample(BasicSampler, input.uv);

    return surfaceColor * float4(colorTint, 0.0f);

}