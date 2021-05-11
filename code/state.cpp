
#include "state.h"

// TODO(Eric): Don't be afraid of deleting all this.

internal void
CreateD3D11RenderTargets(d3d_state *DState)
{
    // Create Framebuffer Render Target and DepthStencilView
    {
        ID3D11Texture2D* d3d11FrameBuffer;
        HRESULT hResult = DState->SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11FrameBuffer);
        assert(SUCCEEDED(hResult));
        
        hResult = DState->Device->CreateRenderTargetView(d3d11FrameBuffer, 0, &DState->FrameBufferView);
        assert(SUCCEEDED(hResult));
        
        D3D11_TEXTURE2D_DESC depthBufferDesc;
        d3d11FrameBuffer->GetDesc(&depthBufferDesc);
        
        d3d11FrameBuffer->Release();
        
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        
        ID3D11Texture2D* depthBuffer;
        DState->Device->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer);
        
        DState->Device->CreateDepthStencilView(depthBuffer, nullptr, &DState->DepthBufferView);
        
        depthBuffer->Release();
    }
    
}

internal void
InitD3D11(d3d_state *DState, HWND hwnd)
{
    // Create D3D11 Device and Context
    {
        ID3D11Device* baseDevice;
        ID3D11DeviceContext* baseDeviceContext;
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG_BUILD)
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        
        HRESULT hResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 
                                            0, creationFlags, 
                                            featureLevels, ARRAYSIZE(featureLevels), 
                                            D3D11_SDK_VERSION, &baseDevice, 
                                            0, &baseDeviceContext);
        if(FAILED(hResult)){
            MessageBoxA(0, "D3D11CreateDevice() failed", "Fatal Error", MB_OK);
            assert(0);
        }
        
        // Get 1.1 interface of D3D11 Device and Context
        hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&DState->Device);
        assert(SUCCEEDED(hResult));
        baseDevice->Release();
        
        hResult = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&DState->DeviceContext);
        assert(SUCCEEDED(hResult));
        baseDeviceContext->Release();
    }
    
#ifdef DEBUG_BUILD
    // Set up debug layer to break on D3D11 errors
    ID3D11Debug *d3dDebug = nullptr;
    DState->Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug);
    if (d3dDebug)
    {
        ID3D11InfoQueue *d3dInfoQueue = nullptr;
        if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
        {
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
            d3dInfoQueue->Release();
        }
        d3dDebug->Release();
    }
#endif
    
    // Create Swap Chain
    {
        // Get DXGI Factory (needed to create Swap Chain)
        IDXGIFactory2* dxgiFactory;
        {
            IDXGIDevice1* dxgiDevice;
            HRESULT hResult = DState->Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);
            assert(SUCCEEDED(hResult));
            
            IDXGIAdapter* dxgiAdapter;
            hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
            assert(SUCCEEDED(hResult));
            dxgiDevice->Release();
            
            DXGI_ADAPTER_DESC adapterDesc;
            dxgiAdapter->GetDesc(&adapterDesc);
            
            OutputDebugStringA("Graphics Device: ");
            OutputDebugStringW(adapterDesc.Description);
            
            hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory);
            assert(SUCCEEDED(hResult));
            dxgiAdapter->Release();
        }
        
        DXGI_SWAP_CHAIN_DESC1 d3d11SwapChainDesc = {};
        d3d11SwapChainDesc.Width = 0; // use window width
        d3d11SwapChainDesc.Height = 0; // use window height
        d3d11SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        d3d11SwapChainDesc.SampleDesc.Count = 1;
        d3d11SwapChainDesc.SampleDesc.Quality = 0;
        d3d11SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        d3d11SwapChainDesc.BufferCount = 2;
        d3d11SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        d3d11SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        d3d11SwapChainDesc.Flags = 0;
        
        HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(DState->Device, hwnd, &d3d11SwapChainDesc, 0, 0, &DState->SwapChain);
        assert(SUCCEEDED(hResult));
        
        dxgiFactory->Release();
    }
}

internal void
InitVertexShaderAndInputLayout(d3d_state *DState, V_SHADER Type, LPCWSTR ShaderFilePath)
{
    // NOTE(Eric): There are 3 ways to Load shaders into our program
    // 1: Load the shader file and compile it at runtime (as we are doing here)
    // 2: Load a precompiled object .cso file, using ReadFileToBlob, and then Create
    // 3: Load from a byte array. Using fxc.exe to compile to a header file, 
    //    and creating a global array variable to access it.
    // Option 3 is the most portable (and we might be able to do it in our build.bat
    
    // Create Vertex Shader
    ID3DBlob* vsBlob;
    ID3DBlob* shaderCompileErrorsBlob;
    
    UINT CompileFlags = 0;
#if defined(DEBUG_BUILD)
    CompileFlags |= D3DCOMPILE_DEBUG;
    CompileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    
    HRESULT hResult = D3DCompileFromFile(ShaderFilePath, nullptr, nullptr, "vs_main", "vs_5_0", CompileFlags, 0, &vsBlob, &shaderCompileErrorsBlob);
    if(FAILED(hResult))
    {
        const char* errorString = NULL;
        if(hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            errorString = "Could not compile shader; file not found";
        else if(shaderCompileErrorsBlob){
            errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
            shaderCompileErrorsBlob->Release();
        }
        MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
        assert(0);
    }
    
    hResult = DState->Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &DState->Shaders.VertexShader[Type]);
    assert(SUCCEEDED(hResult));
    
    // TODO(Eric): One problem with it already is the shared input layouts.
    // If one input layout matches multiple shaders, wouldn't we want to reuse it?
    
    // Create the Input Layout
    switch(Type)
    {
        case V_SHADER_STATIC:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
            {
                { "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
            hResult = DState->Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &DState->Shaders.InputLayout[Type]);
            assert(SUCCEEDED(hResult));
        }break;
        case V_SHADER_TEXTURE:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
            {
                { "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                
                { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };
            hResult = DState->Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &DState->Shaders.InputLayout[Type]);
            assert(SUCCEEDED(hResult));
        } break;
        default:
        {
            assert(0);
        }break;
    }
    
    vsBlob->Release();
}


//~ NOTE(Eric): Pixel Shaders

internal void
InitPixelShader(d3d_state *DState, P_SHADER Type, LPCWSTR ShaderFilePath)
{
    // Create Pixel Shader
    ID3DBlob* psBlob;
    ID3DBlob* shaderCompileErrorsBlob;
    
    HRESULT hResult = D3DCompileFromFile(ShaderFilePath, nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &shaderCompileErrorsBlob);
    if(FAILED(hResult))
    {
        const char* errorString = NULL;
        if(hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            errorString = "Could not compile shader; file not found";
        else if(shaderCompileErrorsBlob){
            errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
            shaderCompileErrorsBlob->Release();
        }
        MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
        assert(0);
    }
    
    hResult = DState->Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &DState->Shaders.PixelShader[Type]);
    assert(SUCCEEDED(hResult));
    psBlob->Release();
    
    
}

