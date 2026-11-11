

#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>

#include "Mesh.h"
#include "Camera.h"

struct SkyVertexShaderData
{
    DirectX::XMFLOAT4X4 View;
    DirectX::XMFLOAT4X4 Projection;
};

class Sky
{
private:
    // Rendering resources
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;

    // Render states
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

    // Sky shaders
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferRing;
    // Geometry
    std::shared_ptr<Mesh> mesh;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
        const wchar_t* right,
        const wchar_t* left,
        const wchar_t* up,
        const wchar_t* down,
        const wchar_t* front,
        const wchar_t* back);


public: 
    Sky(
        std::shared_ptr<Mesh> mesh,
        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
        const wchar_t* right,
        const wchar_t* left,
        const wchar_t* up,
        const wchar_t* down,
        const wchar_t* front,
        const wchar_t* back,
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

    ~Sky();

    void Draw(std::shared_ptr<Camera> camera);
};

