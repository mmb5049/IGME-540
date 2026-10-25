#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "BufferStruct.h"
#include <Windows.h>
#include <d3d11_1.h>
#include <string>
#include <d3d11shadertracing.h>

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();


private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();
	void UINewFrame(float deltaTime);
	void UpdateConstantBuffer(const VertexShaderExternalData& data);
	Microsoft::WRL::ComPtr<ID3D11PixelShader> LoadPixelShader(const wchar_t* path);

	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<GameEntity>> entities;
	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Material>> materials;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelShaderConstantBuffer;

	int activeCameraIndex;
	int number;      // A standard integer variable
	int* ptr;
	float totalTime = 0.0f;

	float localArray[2];
	float* arrayAsPointer;
	DirectX::XMFLOAT4 vectorStruct;
	DirectX::XMFLOAT4 skyColor;
	DirectX::XMFLOAT4 colorTintTriangle;
	DirectX::XMFLOAT3 offsetTriangle;
	DirectX::XMFLOAT4 colorTintQuad;
	DirectX::XMFLOAT3 offsetQuad;
	DirectX::XMFLOAT4 colorTintSpaceship;
	DirectX::XMFLOAT3 offsetSpaceship;
	bool showDemo;
	bool testToggle;
	char testText[128];
	int selectedOption;

	

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	// Textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickTexture;

	// Shared sampler state
	Microsoft::WRL::ComPtr<ID3D11SamplerState> textureSampler;

	

	// Large shared constant buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferRing;

	// Direct3D 11.1 context for binding buffer ranges

	// Ring buffer tracking
	int constantBufferOffset = 0;
	const int CONSTANT_BUFFER_RING_SIZE = 256000;
	const int CONSTANT_BUFFER_ALIGNMENT = 256;
	bool firstConstantBufferAllocation = true;

	// New helper function
	void FillAndBindNextConstantBuffer(
		const void* data,
		int dataSize,
		D3D11_SHADER_TYPE shaderType,
		int slot);
};

