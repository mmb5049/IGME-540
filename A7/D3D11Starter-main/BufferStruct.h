#pragma once

#include <DirectXMath.h>

struct VertexShaderExternalData
{
    DirectX::XMFLOAT4X4 WorldMatrix;
    DirectX::XMFLOAT4X4 ViewMatrix;
    DirectX::XMFLOAT4X4 ProjectionMatrix;

    DirectX::XMFLOAT4 Color;
};