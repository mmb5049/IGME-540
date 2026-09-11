#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

#include <DirectXMath.h>

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
	color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.5f, 1.0f);
	skyColor = XMFLOAT4(0.4f, 0.6f, 0.75f, 1.0f);
	color1 = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	color2 = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	color3 = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	showDemo = false;
	testToggle = false;
	strcpy_s(testText, "Hello ImGui!");
	selectedOption = 0;
	showDemoWindow = true;

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateGeometry();

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
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
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
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

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
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 purple = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 grey = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	// -------------------------
	// Triangle
	// -------------------------

	Vertex triangleVertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), green },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), blue },
	};
	
	unsigned int triangleIndices[] =
	{
		0, 1, 2
	};
	
	meshes.push_back(
		std::make_shared<Mesh>("Triangle",
			triangleVertices, 3,
			triangleIndices, 3
		)
	);


	// -------------------------
	// Square
	// -------------------------

	Vertex squareVertices[] =
	{
		{ XMFLOAT3(-0.7f,  0.6f, 0.0f), blue },
		{ XMFLOAT3(-0.4f,  0.6f, 0.0f), red },
		{ XMFLOAT3(-0.4f, 0.4f, 0.0f), red },
		{ XMFLOAT3(-0.7f, 0.4f, 0.0f), blue }
	};
	
	unsigned int squareIndices[] =
	{
		0, 1, 2,
		0, 2, 3
	};
	
	meshes.push_back(
		std::make_shared<Mesh>("Quad",
			squareVertices, 4,
			squareIndices, 6
		)
	);


	// -------------------------
	// Pentagon
	// -------------------------

	Vertex pentagonVertices[] =
	{
		{ XMFLOAT3(0.5f,  0.6f, 0.0f), black },
		{ XMFLOAT3(0.8f,  0.5f,  0.0f), black },
		{ XMFLOAT3(0.6f, 0.4f, 0.0f), grey },
		{ XMFLOAT3(0.4f, 0.4f, 0.0f), grey },
		{ XMFLOAT3(0.5f,  0.2f, 0.0f), black },
		{ XMFLOAT3(0.8f,  0.3f, 0.0f), black }
	};
	
	unsigned int pentagonIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		3, 2, 4,
		2, 5, 4
	};
	
	meshes.push_back(
		std::make_shared<Mesh>("Spaceship",
			pentagonVertices, 6,
			pentagonIndices, 12
		)
	);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UINewFrame(deltaTime);

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

	// Chaning the sky color
	ImGui::ColorEdit4("Sky Color", &skyColor.x);

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
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), &skyColor.x);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// DRAW geometry
	{
		for (std::shared_ptr<Mesh> mesh : meshes)
		{
			mesh->Draw();
		}
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



