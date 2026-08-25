/*
float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}
*/

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float4 color : COLOR;
};
// Struct representing vertex shader output
struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float4 color : COLOR;
};
// --------------------------------------------------------
// The entry point for our vertex shader
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
    VertexToPixel output;
    output.screenPosition = float4(input.localPosition, 1.0f);
    output.color = input.color;
    return output;
}