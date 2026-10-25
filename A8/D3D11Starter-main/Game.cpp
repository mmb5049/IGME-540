#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStruct.h"
#include "Camera.h"
#include <DirectXMath.h>
#include <WICTextureLoader.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	number = 0;
	ptr = &number;
	localArray[0] = { 0.5f };
	localArray[1] = { 0.5f };
	arrayAsPointer = new float[3];
	vectorStruct = DirectX::XMFLOAT4(10.0f, -2.0f, 99.0f, 0.1f);
	colorTintTriangle = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	offsetTriangle = XMFLOAT3(0.25, 0, 0);
	colorTintQuad = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	offsetQuad = XMFLOAT3(0.25, 0, 0);
	colorTintSpaceship = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	offsetSpaceship = XMFLOAT3(0.25, 0, 0);
	skyColor = XMFLOAT4(0.4f, 0.6f, 0.75f, 1.0f);
	showDemo = false;
	testToggle = false;
	strcpy_s(testText, "Hello ImGui!");
	selectedOption = 0;



	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateGeometry();
	activeCameraIndex = 0;

	cameras.push_back(std::make_shared<Camera>(
		Window::AspectRatio(),
		XMFLOAT3(0.0f, 0.0f, -5.0f),
		XM_PI/4
	));

	cameras.push_back(std::make_shared<Camera>(
		Window::AspectRatio(),
		XMFLOAT3(0.0f, 2.0f, -8.0f),
		XM_PI/3
	));

	cameras.push_back(std::make_shared<Camera>(
		Window::AspectRatio(),
		XMFLOAT3(3.0f, 1.0f, -6.0f),
		XM_PI / 2
	));
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	




	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
	}

	// Set up my constant buffer
	{

		// Set up the shared constant buffer ring
		{
			// Get the Direct3D 11.1 context

			D3D11_BUFFER_DESC cbDesc = {};

			cbDesc.Usage = D3D11_USAGE_DYNAMIC;
			cbDesc.ByteWidth = CONSTANT_BUFFER_RING_SIZE;
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags = 0;
			cbDesc.StructureByteStride = 0;

			Graphics::Device->CreateBuffer(
				&cbDesc,
				nullptr,
				constantBufferRing.GetAddressOf());

			constantBufferOffset = 0;
			firstConstantBufferAllocation = true;
		}
	}
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	delete[] arrayAsPointer;
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPixelShader = LoadPixelShader(FixPath(L"PixelShader.cso").c_str());
	Microsoft::WRL::ComPtr<ID3D11PixelShader> customPixelShader = LoadPixelShader(FixPath(L"CustomPS.cso").c_str());
	Microsoft::WRL::ComPtr<ID3D11PixelShader> normalPixelShader = LoadPixelShader(FixPath(L"DebugNormalsPS.cso").c_str());
	Microsoft::WRL::ComPtr<ID3D11PixelShader> uvPixelShader = LoadPixelShader(FixPath(L"DebugUVsPS.cso").c_str());

	// Loading textures
	DirectX::CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Textures/Wood.png").c_str(),
		nullptr,
		woodTexture.GetAddressOf()
	);
	DirectX::CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Textures/Brick.png").c_str(),
		nullptr,
		brickTexture.GetAddressOf()
	);

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Graphics::Device->CreateSamplerState(
		&samplerDesc,
		textureSampler.GetAddressOf()
	);

	

	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		

		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[3] = {};

		inputElements[0].SemanticName = "POSITION";
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		inputElements[1].SemanticName = "TEXCOORD";
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		inputElements[2].SemanticName = "NORMAL";
		inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,
			3,
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		vertexShader,
		basicPixelShader));

	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		vertexShader,
		basicPixelShader));

	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		vertexShader, 
		basicPixelShader));

	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		vertexShader,
		customPixelShader));

	materials[0]->AddTextureSRV(0, woodTexture);
	materials[0]->AddSampler(0, textureSampler);

	materials[1]->AddTextureSRV(0, woodTexture);
	materials[1]->AddSampler(0, textureSampler);

	materials[2]->AddTextureSRV(0, brickTexture);
	materials[2]->AddSampler(0, textureSampler);
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	meshes.push_back(std::make_shared<Mesh>("Cube",
		FixPath("../../../../Assets/Meshes/cube.obj").c_str()));

	meshes.push_back(std::make_shared<Mesh>("Cylinder",
		FixPath("../../../../Assets/Meshes/cylinder.obj").c_str()));

	meshes.push_back(std::make_shared<Mesh>("Helix",
		FixPath("../../../../Assets/Meshes/helix.obj").c_str()));

	meshes.push_back(std::make_shared<Mesh>("Quad",
		FixPath("../../../../Assets/Meshes/Quad.obj").c_str()));

	meshes.push_back(std::make_shared<Mesh>("Quad Double Sided",
		FixPath("../../../../Assets/Meshes/quad_double_sided.obj").c_str()));

	meshes.push_back(std::make_shared<Mesh>("Sphere",
		FixPath("../../../../Assets/Meshes/sphere.obj").c_str()));

	meshes.push_back(std::make_shared<Mesh>("Torus",
		FixPath("../../../../Assets/Meshes/torus.obj").c_str()));

	for (int i = 0; i < materials.size(); i++)
	{
		for (int y = 0; y < meshes.size(); y++)
		{
			entities.push_back(
				std::make_shared<GameEntity>(meshes[y], materials[i])
			);

			int entityIndex = (int)entities.size() - 1;

			entities[entityIndex]->GetTransform().SetPosition(
				XMFLOAT3(
					4.0f * y - 12.0f,
					4.0f * i - 6.0f,
					0.0f
				)
			);
		}
	}
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
    for (auto& camera : cameras)
    {
        camera->UpdateProjectionMatrix(Window::AspectRatio());
    }
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UINewFrame(deltaTime);

	cameras[activeCameraIndex]->Update(deltaTime);

	ImGui::Begin("Minh - ImGui window");
	// Replace the %f with the next parameter, and format as a float
	ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);

	// Replace each %d with the next parameter, and format as decimal integers
	ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());

	// Draggable slider from 0-100 which reads and updates the variable number
	if (ImGui::SliderInt("Choose a number", &number, 0, 100)) {
		// run this code to console for debug purpose
		printf("New value: %d\n", number);
	}

	// 4 values vectors
	ImGui::DragFloat4("4-component editor", &vectorStruct.x);

	// Changing the sky color
	ImGui::ColorEdit4("Sky Color", &skyColor.x);

	if (ImGui::CollapsingHeader("Color Tint"))
	{
		// Changing the tint color
		ImGui::ColorEdit4("Triangle Tint Color ", &colorTintTriangle.x);
		ImGui::ColorEdit4("Quad Tint Color ", &colorTintQuad.x);
		ImGui::ColorEdit4("Spaceship Tint Color ", &colorTintSpaceship.x);
	}
	
	if (ImGui::CollapsingHeader("Entities"))
	{
		for (int i = 0; i < entities.size(); i++)
		{
			std::string entityName = "Entity " + std::to_string(i);

			if (ImGui::TreeNode(entityName.c_str()))
			{
				Transform& transform = entities[i]->GetTransform();

				// Position
				XMFLOAT3 position = transform.GetPosition();

				if (ImGui::DragFloat3(
					"Position",
					&position.x,
					0.01f
				))
				{
					transform.SetPosition(position);
				}

				// Rotation
				XMFLOAT3 rotation = transform.GetRotation();

				if (ImGui::DragFloat3(
					"Rotation (Radians)",
					&rotation.x,
					0.01f
				))
				{
					transform.SetRotation(rotation);
				}

				// Scale
				XMFLOAT3 scale = transform.GetScale();

				if (ImGui::DragFloat3(
					"Scale",
					&scale.x,
					0.01f
				))
				{
					transform.SetScale(scale);
				}

				ImGui::TreePop();
			}
		}

	}



	// Toggle / Checkbox
	ImGui::Checkbox("Test Toggle", &testToggle);

	// Text box
	ImGui::InputText("Test Text", testText, 99);

	// Dropdown / Combo Box
	const char* options[] = { "Option 1", "Option 2", "Option 3" };
	ImGui::Combo("Test Options", &selectedOption, options, std::size(options));


	if (ImGui::CollapsingHeader("Meshes")) {
		for (std::shared_ptr<Mesh> mesh : meshes)
		{
			std::string headerName = "Meshes: " + mesh->GetName();
			if (ImGui::TreeNode(headerName.c_str()))
			{
				ImGui::Text("Triangles: %d", mesh->GetIndexCount()/3);
				ImGui::Text("Vertices: %d", mesh->GetVertexCount());
				ImGui::Text("Indices: %d", mesh->GetIndexCount());
				ImGui::TreePop();
			}
		}
	}



	if (ImGui::Button("Show/Hide Demo Window"))
	{
		showDemo = !showDemo;
	}
	ImGui::End();
	ImGui::Begin("Camera");

	for (int i = 0; i < cameras.size(); i++)
	{
		char label[32];
		sprintf_s(label, "Camera %d", i + 1);

		if (ImGui::RadioButton(label, activeCameraIndex == i))
		{
			activeCameraIndex = i;
		}
	}

	XMFLOAT3 position =
		cameras[activeCameraIndex]->GetTransform().GetPosition();

	float fov =
		cameras[activeCameraIndex]->GetFieldOfView();

	ImGui::Separator();

	ImGui::Text("Active Camera: %d", activeCameraIndex + 1);

	ImGui::Text(
		"Position: %.2f, %.2f, %.2f",
		position.x,
		position.y,
		position.z
	);

	ImGui::Text(
		"Field of View: %.2f degrees",
		XMConvertToDegrees(fov)
	);

	ImGui::End();



	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}

void Game::UINewFrame(float deltaTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window
	if (showDemo)
	{
		ImGui::ShowDemoWindow();
	}
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	constantBufferOffset = 0;
	firstConstantBufferAllocation = true;
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), &skyColor.x);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}


	// Draw every entity
	for (auto& entity : entities)
	{
		std::shared_ptr<Material> material = entity->GetMaterial();

		// Bind the shaders specified by this entity's material
		Graphics::Context->VSSetShader(
			material->GetVertexShader().Get(),
			0,
			0);

		Graphics::Context->PSSetShader(
			material->GetPixelShader().Get(),
			0,
			0);


		// World matrix
		// Update the vertex shader buffer as before
		VertexShaderExternalData vsData{};
		vsData.WorldMatrix = entity->GetTransform().GetWorldMatrix();
		vsData.ViewMatrix = cameras[activeCameraIndex]->GetViewMatrix();
		vsData.ProjectionMatrix = cameras[activeCameraIndex]->GetProjectionMatrix();
		vsData.Color = material->GetColorTint();

		FillAndBindNextConstantBuffer(
			&vsData,
			sizeof(VertexShaderExternalData),
			D3D11_VERTEX_SHADER,
			0);

		ID3D11ShaderResourceView* textureSRV = woodTexture.Get();

		Graphics::Context->PSSetShaderResources(
			0,
			1,
			&textureSRV
		);

		// Bind sampler to pixel shader slot s0
		ID3D11SamplerState* sampler = textureSampler.Get();

		Graphics::Context->PSSetSamplers(
			0,
			1,
			&sampler
		);

		// Update the pixel shader buffer separately
		PixelShaderExternalData psData{};
		psData.ColorTint = material->GetColorTint();
		psData.TotalTime = totalTime;
		psData.Padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

		FillAndBindNextConstantBuffer(
			&psData,
			sizeof(PixelShaderExternalData),
			D3D11_PIXEL_SHADER,
			0);

		
		entity->GetMaterial()->BindTexturesAndSamplers(
			Graphics::Context.Get());


		// Draw the entity
		entity->Draw();
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();

		// Draw UI
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

void Game::UpdateConstantBuffer(
	const VertexShaderExternalData& data)
{
	D3D11_MAPPED_SUBRESOURCE map{};

	Graphics::Context->Map(
		constantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&map
	);

	memcpy(
		map.pData,
		&data,
		sizeof(VertexShaderExternalData)
	);

	Graphics::Context->Unmap(
		constantBuffer.Get(),
		0
	);
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Game::LoadPixelShader(const wchar_t* path)
{
	// Read the contents of the compiled shader object to a blob
	ID3DBlob* shaderBlob;
	D3DReadFileToBlob(path, &shaderBlob);

	// Create the pixel shader and return it
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Graphics::Device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),	// Pointer to blob's contents
		shaderBlob->GetBufferSize(),	// How big is that data?
		0,								// No classes in this shader
		shader.GetAddressOf());			// Address of the ID3D11PixelShader pointer
	return shader;
}

void Game::FillAndBindNextConstantBuffer(const void* data, int dataSize, D3D11_SHADER_TYPE shaderType, int slot)
{
	// Each allocation starts on a 256-byte boundary
	unsigned int allocationSize =
		(dataSize + CONSTANT_BUFFER_ALIGNMENT - 1)
		/ CONSTANT_BUFFER_ALIGNMENT
		* CONSTANT_BUFFER_ALIGNMENT;

	// Map the shared buffer
	D3D11_MAPPED_SUBRESOURCE mapped = {};

	D3D11_MAP mapType = firstConstantBufferAllocation
		? D3D11_MAP_WRITE_DISCARD
		: D3D11_MAP_WRITE_NO_OVERWRITE;

	Graphics::Context->Map(
		constantBufferRing.Get(),
		0,
		mapType,
		0,
		&mapped);



	// Copy data into the next available section
	memcpy(
		static_cast<char*>(mapped.pData) + constantBufferOffset,
		data,
		dataSize);

	Graphics::Context->Unmap(constantBufferRing.Get(), 0);

	// Convert byte offsets and sizes to 16-byte constant units
	unsigned int firstConstant = constantBufferOffset / 16;
	unsigned int numConstants = allocationSize / 16;

	ID3D11Buffer* buffer = constantBufferRing.Get();

	// Bind only the section used by this shader
	if (shaderType == D3D11_VERTEX_SHADER)
	{
		Graphics::Context->VSSetConstantBuffers1(
			slot,
			1,
			&buffer,
			&firstConstant,
			&numConstants);
	}
	else if (shaderType == D3D11_PIXEL_SHADER)
	{
		Graphics::Context->PSSetConstantBuffers1(
			slot,
			1,
			&buffer,
			&firstConstant,
			&numConstants);
	}

	// Advance to the next available section
	constantBufferOffset += allocationSize;
	firstConstantBufferAllocation = false;
}
 
