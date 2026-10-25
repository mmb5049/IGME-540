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

void Material::AddTextureSRV(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs[slot] = srv;
}

void Material::AddSampler(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
    samplers[slot] = sampler;
}

void Material::BindTexturesAndSamplers(ID3D11DeviceContext* context)
{
    for (const auto& entry : textureSRVs)
    {
        unsigned int slot = entry.first;
        ID3D11ShaderResourceView* srv = entry.second.Get();

        context->PSSetShaderResources(slot, 1, &srv);
    }

    for (const auto& entry : samplers)
    {
        unsigned int slot = entry.first;
        ID3D11SamplerState* sampler = entry.second.Get();

        context->PSSetSamplers(slot, 1, &sampler);
    }

}

DirectX::XMFLOAT2 Material::GetUVScale()
{
    return uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
    return uvOffset;
}

void Material::SetUVScale(DirectX::XMFLOAT2 scale)
{
    uvScale = scale;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 offset)
{
    uvOffset = offset;
}

const std::unordered_map<unsigned int, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& Material::GetTextureSRVs()
{
    return textureSRVs;
}

