#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <unordered_map>
#include "Camera.h"
#include "Transform.h"

class Material
{
private:
    DirectX::XMFLOAT4 colorTint;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

    std::unordered_map<unsigned int,Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;

    std::unordered_map<unsigned int, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

public:
    Material(
        DirectX::XMFLOAT4 colorTint,
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

    DirectX::XMFLOAT4 GetColorTint();
    Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
    Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();

    void SetColorTint(DirectX::XMFLOAT4 colorTint);
    void SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader);
    void SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

    void AddTextureSRV(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);

    void AddSampler(unsigned int slot,Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);

    void BindTexturesAndSamplers(ID3D11DeviceContext* context);

};

