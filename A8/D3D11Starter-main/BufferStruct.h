#pragma once

#include <DirectXMath.h>

struct VertexShaderExternalData
{
    DirectX::XMFLOAT4X4 WorldMatrix;
    DirectX::XMFLOAT4X4 ViewMatrix;
    DirectX::XMFLOAT4X4 ProjectionMatrix;

    DirectX::XMFLOAT4 Color;
};

struct PixelShaderExternalData
{
    DirectX::XMFLOAT4 ColorTint;
    float TotalTime;
    DirectX::XMFLOAT3 Padding;

    DirectX::XMFLOAT2 UVScale;
    DirectX::XMFLOAT2 UVOffset;
};