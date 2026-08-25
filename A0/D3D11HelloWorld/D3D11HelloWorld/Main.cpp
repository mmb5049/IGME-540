// Include library files
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Include header files
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <Windows.h>

// Global data to track
unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;

// Global D3D API objects
Microsoft::WRL::ComPtr<ID3D11Device1> Device;
Microsoft::WRL::ComPtr<ID3D11DeviceContext1> Context;
Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BackBufferRTV;

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

// -----------------------------------------
// The update/simulation phase of the game loop
// -----------------------------------------
void Update()
{
	if (GetKeyState(VK_ESCAPE) & 0x8000)
		PostQuitMessage(0);
}
// -----------------------------------------
// The draw/rendering phase of the game loop
// -----------------------------------------
void Draw()
{
	// Clear the window to a particular color
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	Context->ClearRenderTargetView(BackBufferRTV.Get(), color);
	// Actually draw 3 indices worth of geometry
	Context->DrawIndexed(3, 0, 0);
	// Show the results in the window
	SwapChain->Present(1, 0);
	Context->OMSetRenderTargets(1, BackBufferRTV.GetAddressOf(), 0);
}
// -----------------------------------------
// Operating system window callback function
// -----------------------------------------
LRESULT ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	{
		// Check the incoming message and handle any we care about
		switch (uMsg)
		{
			// This is the message that signifies the window closing
		case WM_DESTROY:
			PostQuitMessage(0); // Send a quit message to our own program
			return 0;
		}
		// Let Windows handle any messages we're not touching
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
// -----------------------------------------
// Program entry point instead of main()
// -----------------------------------------
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// Fill out a description ("class") for the window we want
	WNDCLASS wndClass{};
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = ProcessMessage; // Specifying our app’s callback function
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"HelloWorldD3D11Class";
	// Attempt to register the window class we've defined
	if (!RegisterClass(&wndClass))
	{
		// Get the most recent error. If the window "class" already exists
		// that's fine. Otherwise, we have a problem and should exit.
		DWORD error = GetLastError();
		if (error != ERROR_CLASS_ALREADY_EXISTS)
			return HRESULT_FROM_WIN32(error);
	}
	// Adjust width & height of the window's inner ("client") area
	RECT clientRect;
	SetRect(&clientRect, 0, 0, windowWidth, windowHeight);
	AdjustWindowRect(
		&clientRect,
		WS_OVERLAPPEDWINDOW,
		false);
	// Center the window on the screen
	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);
	int centeredX = (desktopRect.right / 2) - (clientRect.right / 2);
	int centeredY = (desktopRect.bottom / 2) - (clientRect.bottom / 2);
	// Ask Windows to create the window. This returns a window
	// "handle" (a unique ID), which we'll keep around for later
	HWND windowHandle = CreateWindow(
		wndClass.lpszClassName,
		L"D3D11 Hello World",
		WS_OVERLAPPEDWINDOW,
		centeredX,
		centeredY,
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top,
		0,
		0,
		hInstance,
		0);
	// Ensure the window was created properly
	if (windowHandle == NULL)
	{
		DWORD error = GetLastError();
		return HRESULT_FROM_WIN32(error);
	}
	// Finally make the window visible
	ShowWindow(windowHandle, SW_SHOW);

	// Create a description of our rendering surface
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = windowWidth;
	swapDesc.BufferDesc.Height = windowHeight;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.Flags = 0;
	swapDesc.OutputWindow = windowHandle;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Windowed = true;
	// Attempt to initialize Direct3D 11.1
	D3D_FEATURE_LEVEL desiredLevel = D3D_FEATURE_LEVEL_11_1;
	D3D_FEATURE_LEVEL actualLevel;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		D3D11_CREATE_DEVICE_DEBUG,
		&desiredLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		SwapChain.GetAddressOf(),
		(ID3D11Device**)Device.GetAddressOf(),
		&actualLevel,
		(ID3D11DeviceContext**)Context.GetAddressOf());
	// If it failed, exit using the error code
	if (FAILED(hr))
		return hr;
	// Grab the reference to the first buffer
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTexture;
	SwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)backBufferTexture.GetAddressOf());
	// Create a render target view so we can render into the buffer
	Device->CreateRenderTargetView(
		backBufferTexture.Get(),
		0,
		BackBufferRTV.GetAddressOf());

	// Define a viewport to render into the whole window
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)windowWidth;
	viewport.Height = (float)windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	// Load compiled shader code
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;
	D3DReadFileToBlob(L"../x64/Debug/PixelShader.cso", &pixelShaderBlob);
	D3DReadFileToBlob(L"../x64/Debug/VertexShader.cso", &vertexShaderBlob);
	// Create the actual Direct3D shaders objects
	Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		0,
		pixelShader.GetAddressOf());
	Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		0,
		vertexShader.GetAddressOf());
	// Set up an input layout to describe our vertex data
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};
	// Set up the first element - a position, which is three 32-bit float values
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElements[0].SemanticName = "POSITION";
	inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	// Set up the second element - a color, which is 4 more 32-bit float values
	inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElements[1].SemanticName = "COLOR";
	inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	// Create the input layout, verifying against actual shader code
	Device->CreateInputLayout(
		inputElements,
		2,
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		inputLayout.GetAddressOf());
	// Set up the various stages of the rendering pipeline
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context->IASetInputLayout(inputLayout.Get());
	Context->VSSetShader(vertexShader.Get(), 0, 0);
	Context->RSSetViewports(1, &viewport);
	Context->PSSetShader(pixelShader.Get(), 0, 0);
	Context->OMSetRenderTargets(1, BackBufferRTV.GetAddressOf(), 0);

	// API objects representing GPU buffers that will hold our geometry
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	// Vertex buffer setup
	{
		// Color definitions
		DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		// Actual vertex data to be placed on the GPU
		Vertex vertices[] =
		{
		{ DirectX::XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ DirectX::XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
		};
		// First, we need to describe the buffer we want to make on the GPU
		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * 3;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		// Create the proper struct to hold the initial vertex data
		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertices;
		// Actually create the buffer on the GPU with the initial data
		Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	}

	// Index buffer setup
	{
		// Index data for optimized rendering (overkill in this demo)
		unsigned int indices[] = { 0, 1, 2 };
		// Describe the buffer, as we did above, with some differences
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * 3;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		// Specify the initial data for this buffer, similar to above
		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indices;
		// Actually create the buffer with the initial data
		Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	}
	// Tell D3D to use these particular buffers when drawing
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);



	//Windows message loop(and our game loop)
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		// Determine if there is a message from the operating system
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate and dispatch the message
			// to our custom ProcessMessage function
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Game loop
			Update();
			Draw();
		}
	}
	return (HRESULT)msg.wParam;
}