#pragma once

#include <DirectXMath.h>

struct VertexShaderExternalData
{
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT4X4 WorldMatrix;
};