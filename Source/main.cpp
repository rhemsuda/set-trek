/*
	File Name:		main.cpp
	Description:	This file is the main method of the application. It handles the initialization of DirectX3d and
					handles the linking and dynamic code-recompilation for the GameUpdateAndRender loop on the game side.
	Programmer:		Kyle Jensen
	Date:			April 14, 2017
*/

#include <windows.h>
#include "../Include/Game.h"


IDXGISwapChain* swapChain;

//Screen information
int screenWidth = 800;
int screenHeight = 600;

//This struct is used for dynamic code reloading. It tracks the module handle for the Game DLL, a pointer
//to the GameUpdateAndRender method, and the last write time.
struct GameCode
{
	HMODULE gameDLL;
	_GameUpdateAndRender* GameUpdateAndRender;

	FILETIME lastWriteTime;
};

//Function: CompileShaderFromFile()
//Description: This method takes in a shader filename and loads the shader buffer.
//Returns: void.
void CompileShaderFromFile(wchar_t* shaderFileName, ID3D10Blob** shaderBuffer, bool pixel)
{
    D3DCompileFromFile((LPCWSTR)shaderFileName, NULL, NULL, 
		pixel ? "TexturePixelShader" : "TextureVertexShader",
        pixel ? "ps_5_0" : "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, shaderBuffer, 0
	);
}
//Function: CompileShaderFromFile()
//Description: If the game DLL doesnt exist or if the current write time on the DLL is greater than the previous write time
//We want to free the dll data from the gameCode structure, and then copy the dll to temporary memory. We can then
//swap out the GameUpdateAndRender prodedure of the dll using the GetProcAddress method and reset the last write time.
//Returns: void.
void TryReloadGameCode(GameCode* gameCode, char* gameDLLPath, char* gameTempDLLPath)
{
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    GetFileAttributesExA(gameDLLPath, GetFileExInfoStandard, &fileInfo);
    FILETIME currentWriteTime = fileInfo.ftLastWriteTime;

	//If the game code hasnt been initialized or needs reinitialization, we free the library memory, 
	//copy the dll over to temporary memory and reinitialize the GameUpdateAndRender method from the DLL.
	if (!gameCode->gameDLL || CompareFileTime(&currentWriteTime, &gameCode->lastWriteTime) == 1)
    {
        FreeLibrary(gameCode->gameDLL);
        CopyFileA(gameDLLPath, gameTempDLLPath, FALSE);
        gameCode->gameDLL = LoadLibraryA(gameTempDLLPath);
        gameCode->GameUpdateAndRender = (_GameUpdateAndRender*)GetProcAddress(gameCode->gameDLL, "GameUpdateAndRender");
        gameCode->lastWriteTime = currentWriteTime;
    }
}


//Function: WinProc()
//Description: This method is the Window Procedure called by the WinMain method. It drives the resizing of the window in our application.
//Returns: LRESULT = result of the window procedure.
LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{       
    LRESULT result = 0;

    switch(message)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;
        
		case WM_SIZE:
		{
			screenWidth = LOWORD(lParam);
			screenHeight = HIWORD(lParam);
		}
		break;

        default:
            result = DefWindowProcA(window, message, wParam, lParam);
        break;
    }

    return result;
}


//Function: WinMain()
//Description: This is the main method.
//Returns: int = result of the main method.
int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    WNDCLASSA windowClass = {};

	windowClass.style = CS_HREDRAW|CS_VREDRAW;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hInstance = instance;
	windowClass.hCursor = LoadCursor(0, IDC_ARROW);
	windowClass.lpszClassName = "SetTrekWindowClass";

    if(RegisterClassA(&windowClass))
    {
        HWND window = CreateWindowExA
		(
			0, windowClass.lpszClassName, "Set Trek",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            800, 600, 0, 0, instance, 0
		);

		if (window)
		{
			HRESULT result;
			bool vsyncEnabled = true;

			//Define matrices
			XMMATRIX projectionMatrix;
			XMMATRIX worldMatrix;
			XMMATRIX orthoMatrix;
			XMMATRIX viewMatrix;

			//Adding definitions here that will be used "globally"
			ID3D11Device* device;
			ID3D11DeviceContext* deviceContext;
			ID3D11RenderTargetView* renderTargetView;


			#pragma region Refresh Rate

				IDXGIFactory* factory = 0;
				IDXGIAdapter* adapter = 0;
				IDXGIOutput* adapterOutput = 0;
				DXGI_ADAPTER_DESC adapterDesc;
				DXGI_MODE_DESC* displayModeList = 0;
				DXGI_RATIONAL refreshRate;

				int videoCardMemory = 0;
				unsigned int numModes = 0;

				result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
				if (FAILED(result))
					return false;

				result = factory->EnumAdapters(0, &adapter);
				if (FAILED(result))
					return false;

				result = adapter->EnumOutputs(0, &adapterOutput);
				if (FAILED(result))
					return false;

				result = adapter->GetDesc(&adapterDesc);
				if (FAILED(result))
					return false;

				// TODO: Proper dxgi format for 32b color
				result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
				if (FAILED(result))
					return false;

				displayModeList = new DXGI_MODE_DESC[numModes];

				result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
				if (FAILED(result))
					return false;

				bool foundRefreshRate = false;

				for (unsigned int i = 0; i < numModes; i++)
				{
					if (displayModeList[i].Width == (unsigned int)screenWidth
						&& displayModeList[i].Height == (unsigned int)screenHeight)
					{
						refreshRate = displayModeList[i].RefreshRate;
						foundRefreshRate = true;
						break;
					}
				}

				//Set the video card memory in megabytes
				videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

				//Release display mode list
				delete[] displayModeList;
				displayModeList = 0;

				// Release the adapter output.
				adapterOutput->Release();
				adapterOutput = 0;

				// Release the adapter.
				adapter->Release();
				adapter = 0;

				// Release the factory.
				factory->Release();
				factory = 0;

			#pragma endregion


			#pragma region Swap Chain Initiliazation

				DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	
				ID3D11Texture2D* backBufferPtr;

				D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

				swapChainDesc.BufferCount = 1;									//Set to single back buffer
				swapChainDesc.Windowed = true;									//Set the application to be in windowed mode          
				swapChainDesc.BufferDesc.Width = screenWidth;					//Set the screenWidth of the buffer
				swapChainDesc.BufferDesc.Height = screenHeight;					//Set the screenHeight of the buffer          
				swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//Set regular 32-bit surface for the back buffer.

				if (foundRefreshRate)
				{
					swapChainDesc.BufferDesc.RefreshRate.Numerator = refreshRate.Numerator;
					swapChainDesc.BufferDesc.RefreshRate.Denominator = refreshRate.Denominator;
				}
				else
				{
					swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
					swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
				}

				//Use a render target view as the ouput for the buffer
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

				//Set the handle for the window to render to		
				swapChainDesc.OutputWindow = window;

				//Disable multisampling
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;

				//Set the scanline ordering and scaling to unspecified
				swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

				//Discard the back buffer contents after presenting.
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

				//Set flags to zero, we arent using them
				swapChainDesc.Flags = 0;

				result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
					D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
				if (FAILED(result))
					return false;

				result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
				if (FAILED(result))
					return false;

				// Create the render target view with the back buffer pointer.
				result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
				if (FAILED(result))
					return false;

				// Release pointer to the back buffer as we no longer need it.
				backBufferPtr->Release();
				backBufferPtr = 0;

			#pragma endregion


			#pragma region Depth Buffer Initialization

				//This region of code initializes the depth buffer for our polygons to be rendered in 3D space.
				//I am adding a stencil buffer to the depth buffer in order to achieve effects such as motion blur.

				D3D11_TEXTURE2D_DESC depthBufferDesc = {};
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
				ID3D11Texture2D* depthStencilBuffer;
				ID3D11DepthStencilState* depthStencilState;
				ID3D11DepthStencilView* depthStencilView;

				// Set up the description of the depth buffer.
				depthBufferDesc.Width = screenWidth;
				depthBufferDesc.Height = screenHeight;
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.ArraySize = 1;
				depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //24 bits for depth, 8 for stencil
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Usage = D3D11_USAGE_DEFAULT; //Requires read and write access from GPU
				depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //Binds a texture as a depth-stencil target for output-merger state
				depthBufferDesc.CPUAccessFlags = 0;
				depthBufferDesc.MiscFlags = 0;

				//Create the texture for the depth buffer using the filled out description
				result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
				if (FAILED(result))
					return false;

				//Set up the description of the stencil state
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
				depthStencilDesc.StencilEnable = true;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;

				//Stencil operations for front-face pixels
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				//Stencil operations for back-face pixels
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				//Create the depth stencil state
				result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
				if (FAILED(result))
					return false;

				// Set the depth stencil state.
				deviceContext->OMSetDepthStencilState(depthStencilState, 1);

				// Set up the depth stencil view description.
				depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				depthStencilViewDesc.Texture2D.MipSlice = 0;

				// Create the depth stencil view.
				result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
				if (FAILED(result))
					return false;

				// Bind the render target view and depth stencil buffer to the output render pipeline.
				deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

			#pragma endregion


			#pragma region Rasterization

				D3D11_RASTERIZER_DESC rasterDesc;
				ID3D11RasterizerState* rasterState;

				// Setup the raster description which will determine how and what polygons will be drawn.
				rasterDesc.AntialiasedLineEnable = false;
				rasterDesc.CullMode = D3D11_CULL_BACK;
				rasterDesc.DepthBias = 0;
				rasterDesc.DepthBiasClamp = 0.0f;
				rasterDesc.DepthClipEnable = true;
				rasterDesc.FillMode = D3D11_FILL_SOLID;
				rasterDesc.FrontCounterClockwise = false;
				rasterDesc.MultisampleEnable = false;
				rasterDesc.ScissorEnable = false;
				rasterDesc.SlopeScaledDepthBias = 0.0f;

				// Create the rasterizer state from the description we just filled out.
				result = device->CreateRasterizerState(&rasterDesc, &rasterState);
				if (FAILED(result))
					return false;

				// Now set the rasterizer state.
				deviceContext->RSSetState(rasterState);

			#pragma endregion


			#pragma region Viewport/Projection

				D3D11_VIEWPORT viewport = {};
				float fieldOfView, screenAspect;
				float screenDepth = 1000.0f;
				float screenNear = 0.1f;

				// Setup the viewport for rendering
				viewport.Width = (float)screenWidth;
				viewport.Height = (float)screenHeight;
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;
				viewport.TopLeftX = 0.0f;
				viewport.TopLeftY = 0.0f;

				//Create the viewport
				deviceContext->RSSetViewports(1, &viewport);

			#pragma endregion


			#pragma region Load Vertex and Pixel Shaders

				//TODO: Extend this into a Shader object that will encapsulate the loading and compiling of the shader on the GPU,
				//to allow different shaders to be bound to different models. For the purposes of this assignment we only need 1 basic shader.

				ID3D11VertexShader* vertexShader = 0;
				ID3D11PixelShader* pixelShader = 0;
				ID3D11InputLayout* layout = 0;

				ID3D10Blob* errorMessage = 0;
				ID3D10Blob* vertexShaderBuffer = 0;
				ID3D10Blob* pixelShaderBuffer = 0;

				D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
				unsigned int numElements;

				TCHAR filepath[MAX_PATH];
				GetModuleFileName(NULL, filepath, MAX_PATH);

				LPWSTR vsFilename = L"Assets//Shaders//texture.vs";
				LPWSTR psFilename = L"Assets//Shaders//texture.ps";
				CompileShaderFromFile(vsFilename, &vertexShaderBuffer, false);
				CompileShaderFromFile(psFilename, &pixelShaderBuffer, true);

				// Create the vertex shader from the buffer.
				result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
				if (FAILED(result))
					return false;

				// Create the pixel shader from the buffer.
				result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
				if (FAILED(result))
					return false;

				//Set up the layout of the data in the shader (matches format of shader file)
				polygonLayout[0].SemanticName = "POSITION";
				polygonLayout[0].SemanticIndex = 0;
				polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				polygonLayout[0].InputSlot = 0;
				polygonLayout[0].AlignedByteOffset = 0;
				polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				polygonLayout[0].InstanceDataStepRate = 0;

				polygonLayout[1].SemanticName = "TEXCOORD";
				polygonLayout[1].SemanticIndex = 0;
				polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
				polygonLayout[1].InputSlot = 0;
				polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				polygonLayout[1].InstanceDataStepRate = 0;

				// Get a count of the elements in the layout.
				numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

				// Create the vertex input layout.
				result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
					vertexShaderBuffer->GetBufferSize(), &layout);
				if (FAILED(result))
					return false;

				// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
				vertexShaderBuffer->Release();
				vertexShaderBuffer = 0;
				pixelShaderBuffer->Release();
				pixelShaderBuffer = 0;

			#pragma endregion

	
			#pragma region Texture Sampling

				D3D11_SAMPLER_DESC samplerDesc;
				ID3D11SamplerState* sampleState = 0;

				//Create texture sampler state description
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.MipLODBias = 0.0f;
				samplerDesc.MaxAnisotropy = 1;
				samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				samplerDesc.BorderColor[0] = 0;
				samplerDesc.BorderColor[1] = 0;
				samplerDesc.BorderColor[2] = 0;
				samplerDesc.BorderColor[3] = 0;
				samplerDesc.MinLOD = 0;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

				//Create texture sampler state
				result = device->CreateSamplerState(&samplerDesc, &sampleState);
				if (FAILED(result))
					return false;

			#pragma endregion


			#pragma region Shader Input and Alpha Blending

				deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				deviceContext->IASetInputLayout(layout);
				deviceContext->VSSetShader(vertexShader, NULL, 0);
				deviceContext->PSSetShader(pixelShader, NULL, 0);

				ID3D11BlendState* blendState = NULL;

				//Initialize blend description for alpha blending
				D3D11_BLEND_DESC blendDesc = {};
				blendDesc.RenderTarget[0].BlendEnable = TRUE;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				device->CreateBlendState(&blendDesc, &blendState);

				float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				UINT sampleMask = 0xffffffff;

				deviceContext->OMSetBlendState(blendState,0, sampleMask);

			#pragma endregion
			
			#pragma region Matrix Buffer
			
				D3D11_BUFFER_DESC matrixBufferDesc;
				ID3D11Buffer* matrixBuffer;

				// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
				matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
				matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				matrixBufferDesc.MiscFlags = 0;
				matrixBufferDesc.StructureByteStride = 0;

				// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
				device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
			
			#pragma endregion

			#pragma region SpriteBatch Initialization

				std::unique_ptr<SpriteBatch> spriteBatch;
				std::unique_ptr<SpriteFont> spriteFontLucida24;
				std::unique_ptr<SpriteFont> spriteFontLucida56;
				spriteBatch.reset(new SpriteBatch(deviceContext));
				spriteFontLucida24.reset(new SpriteFont(device, L"Assets//Fonts//lucida.spritefont"));
				spriteFontLucida56.reset(new SpriteFont(device, L"Assets//Fonts//lucida56.spritefont"));
				
			#pragma endregion

			#pragma region Initialize Sound Drivers

				IDirectSound8* directSound;
				IDirectSoundBuffer* primaryBuffer;

				DSBUFFERDESC soundBufferDesc;
				WAVEFORMATEX waveFormat;

				//Initialize direct sound interface to sound device.
				result = DirectSoundCreate8(NULL, &directSound, NULL);
				if (FAILED(result))
					return false;

				//Set cooperative level to priority to allow the format of the sound buffer to be modified.
				result = directSound->SetCooperativeLevel(window, DSSCL_PRIORITY);
				if (FAILED(result))
					return false;

				//Setup primary buffer description.
				soundBufferDesc.dwSize = sizeof(DSBUFFERDESC);
				soundBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
				soundBufferDesc.dwBufferBytes = 0;
				soundBufferDesc.dwReserved = 0;
				soundBufferDesc.lpwfxFormat = NULL;
				soundBufferDesc.guid3DAlgorithm = GUID_NULL;

				//Get control of the primary sound buffer on the default sound device.
				result = directSound->CreateSoundBuffer(&soundBufferDesc, &primaryBuffer, NULL);
				if (FAILED(result))

				//Setup the format of the primary sound bufffer (.WAV file recorded at 44,100 samples per second in 16-bit stereo).
				waveFormat.wFormatTag = WAVE_FORMAT_PCM;
				waveFormat.nSamplesPerSec = 44100;
				waveFormat.wBitsPerSample = 16;
				waveFormat.nChannels = 2;
				waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
				waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
				waveFormat.cbSize = 0;

				//Set the primary buffer to be the wave format specified.
				result = primaryBuffer->SetFormat(&waveFormat);
				if (FAILED(result))
					return false;

			#pragma endregion

			#pragma region Main Game Loop

				//Create a new struct to hold the DLL information about the Game code
				GameCode gameCode = {};

				char* gameDLLPath = "Game.dll";
				char* gameTempDLLPath = "Gametemp.dll";

				//Try Dynamic code reload, being the first time this should pass and start the program
				TryReloadGameCode(&gameCode, gameDLLPath, gameTempDLLPath);

				//Initialize the game state information with the rendering information we previously setup
				GameState gameState = {};
				gameState.swapChain = swapChain;
				gameState.matrixBuffer = matrixBuffer;
				gameState.spriteBatch = spriteBatch.get();
				gameState.spriteFontLucida24 = spriteFontLucida24.get();
				gameState.spriteFontLucida56 = spriteFontLucida56.get();
				gameState.vertexShader = vertexShader;
				gameState.pixelShader = pixelShader;
				gameState.layout = layout;
				gameState.rasterState = rasterState;
				gameState.sampleState = sampleState;
				gameState.blendState = blendState;
				gameState.directSound = directSound;
				gameState.primaryBuffer = primaryBuffer;
				gameState.levelState = LevelState::Start;

				Input gameInput = {};
				MouseInput currentMouseInput = {};
				MouseInput lastMouseInput = {};

				bool running = true;
				while (running)
				{
					TryReloadGameCode(&gameCode, gameDLLPath, gameTempDLLPath);

					MSG message;
					while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
					{
						if (message.message == WM_QUIT)
							running = false;

						TranslateMessage(&message);
						DispatchMessageA(&message);
					}		

					POINT mousePos;
					GetCursorPos(&mousePos);

					MouseInput currentMouseInput = {};
					MouseInput lastMouseInput = {};

					//Convert mouse points to client pos
					ScreenToClient(window, &mousePos);
					currentMouseInput.x = currentMouseInput.x = mousePos.x;
					currentMouseInput.y = currentMouseInput.y = mousePos.y;

					//Set the mouse click input
					currentMouseInput.downL = GetAsyncKeyState(VK_LBUTTON) & 0x0F;
					currentMouseInput.downR = GetAsyncKeyState(VK_RBUTTON) & 0x0F;

					//Handle full clicks rather than mouse down
					currentMouseInput.clickedL = (currentMouseInput.downL && !lastMouseInput.downL);
					currentMouseInput.clickedR = (currentMouseInput.downR && !lastMouseInput.downR);

					//Set mouse input
					gameInput.mouse = currentMouseInput;

					//Set keyboard input
					gameInput.enter = GetAsyncKeyState(VK_RETURN) & 0x0F;
					gameInput.key1 = GetAsyncKeyState('1') & 0x0F;
					gameInput.key2 = GetAsyncKeyState('2') & 0x0F;
					gameInput.key3 = GetAsyncKeyState('3') & 0x0F;
					gameInput.key4 = GetAsyncKeyState('4') & 0x0F;
					gameInput.keyE = GetAsyncKeyState('E') & 0x0F;

					//If the game update and render method was successfully loaded from DLL into the program, run it :D 
					if (gameCode.GameUpdateAndRender)
					{
						gameCode.GameUpdateAndRender(&gameState, deviceContext, device, renderTargetView, depthStencilView, screenWidth, screenHeight, gameInput);
					}
						
					//Set input information	
					lastMouseInput = currentMouseInput;
				}
	
			#pragma endregion   

        }
    }

    return 0;
}
