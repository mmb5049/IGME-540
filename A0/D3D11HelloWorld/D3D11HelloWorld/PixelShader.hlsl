/*
float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
*/

// Struct representing pixel shader input
struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float4 color : COLOR;
};
// --------------------------------------------------------
// The entry point for our pixel shader
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    return input.color;
}

