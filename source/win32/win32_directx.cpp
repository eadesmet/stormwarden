


// TODO(Eric): Figure this out.
// Either compile D3D in CPP and export it to use within C
// OR just go with OpenGL once again..
// ORRRR convert this entire project to use cpp instead.

// TODO(Eric): Actual TODO here.
// Hmm.. It should be simple enough to load D3D cpp code just like the game code
// That seems easy enough, and I get the best of all worlds, right?
// OR, just convert the app.dll to be cpp, and use directx there????

//// NOTE(Eric): The next day: just going back to opengl for now.
/*
#if 1

#ifdef __cplusplus
extern "C" {
    
    internal void
        InitD3D11(DX_State *DState, HWND hwnd)
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
    
}
#endif












#else
internal void
InitD3D11(DX_State *State, HWND hwnd)
{
    // Create D3D11 Device and Context
    {
        ID3D11Device* baseDevice;
        ID3D11DeviceContext* baseDeviceContext;
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DBUILD_WIN32)
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        
        HRESULT hResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 
                                            0, creationFlags, 
                                            featureLevels, ARRAYSIZE(featureLevels), 
                                            D3D11_SDK_VERSION, &baseDevice, 
                                            0, &baseDeviceContext);
        Assert(SUCCEEDED(hResult));
        
        // Get 1.1 interface of D3D11 Device and Context
        //hResult = ID3D11Device_QueryInterface(baseDevice, IID_ID3D11Device1, (void**)&State->Device);
        //Assert(SUCCEEDED(hResult));
        //IDXGIDevice_Release(baseDevice);
        
        //hResult = ID3D11DeviceContext_QueryInterface(State->DeviceContext, IID_ID3D11DeviceContext, (void**)&State->DeviceContext);
        //Assert(SUCCEEDED(hResult));
        //ID3D11DeviceContext_Release(baseDeviceContext);
    }
    
#ifdef DBUILD_WIN32
    // Set up debug layer to break on D3D11 errors
    ID3D11Debug *d3dDebug = nullptr;
    State->Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug);
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
            HRESULT hResult = IDXGIDevice_QueryInterface(dxgiDevice, IID_IDXGIDevice1, (void**)&dxgiDevice);
            Assert(SUCCEEDED(hResult));
            
            IDXGIAdapter* dxgiAdapter;
            hResult = IDXGIDevice_GetAdapter(dxgiDevice, &dxgiAdapter);
            Assert(SUCCEEDED(hResult));
            IDXGIDevice_Release(dxgiDevice);
            
            DXGI_ADAPTER_DESC adapterDesc;
            IDXGIAdapter_GetDesc(dxgiAdapter, &adapterDesc);
            
            Log("Graphics Device: ");
            Log((char *)adapterDesc.Description);// TODO(Eric): Does this cast work? WCHAR[128] to char*
            
            hResult = IDXGIAdapter_GetParent(dxgiAdapter, IID_IDXGIFactory2, (void**)&dxgiFactory);
            Assert(SUCCEEDED(hResult));
            IDXGIAdapter_Release(dxgiAdapter);

            HRESULT hresult = CreateDXGIFactory2(0, &IID_IDXGIFactory2,
                                                 &dxgiFactory);
        }
        
        DXGI_SWAP_CHAIN_DESC1 d3d11SwapChainDesc = {0};
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
        
        // Swapchain requires Factory5????
        
        HRESULT hResult = IDXGIFactory2_CreateSwapChain(State->Device, hwnd, &d3d11SwapChainDesc, 0, 0, &State->SwapChain);
        Assert(SUCCEEDED(hResult));
        
        DXGIFactory2_Release(dxgiFactory);
    }
    
}
#endif

*/