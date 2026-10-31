#include "ShaderStruct.hlsli"

cbuffer externalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    matrix worldInvTranspose;
  
};

struct VertexShaderInput
{ 

	float3 localPosition	: POSITION;    
    float2 uv				: TEXCOORD;
    float3 normal			: NORMAL; 
};



VertexToPixel main( VertexShaderInput input )
{

	VertexToPixel output;

    matrix wvp = mul(projection, mul(view, world));
	
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

    output.uv = input.uv;
    output.normal = mul((float3x3) worldInvTranspose, input.normal);
    output.worldPosition = mul(world, float4(input.localPosition, 1.0f)).xyz;

	return output;
}