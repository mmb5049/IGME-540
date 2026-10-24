#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader)
{
    this->colorTint = colorTint;
    this->vertexShader = vertexShader;
    this->pixelShader = pixelShader;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPixelShader()
{
    return pixelShader;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
    this->colorTint = colorTint;
}

void Material::SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader)
{
    this->vertexShader = vertexShader;
}

void Material::SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader)
{
    this->pixelShader = pixelShader;
}
