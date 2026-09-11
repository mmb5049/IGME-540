#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Vertex.h"
#include "Graphics.h"

class Mesh
{
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    int indexCount;
    int vertexCount;

    std::string name;

public:
    Mesh(std::string name, Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount);
    ~Mesh();
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
    int GetIndexCount();
    int GetVertexCount();
    std::string GetName();
    void Draw();
};

