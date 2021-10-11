
// TODO(Eric): 7/7/2021: Look at refterm as a reference for getting directx up and running in C
// and apply it here. 
// - Have the ability to switch between directx and opengl.
// - make a generic 'renderer', basically with an if(opengl) {use opengl} else if (directx) {use directx}
//   - Though, why would I need this?
// - Go through the directx luna book and _make some progress!_

//#include "win32_directx.h"

#include "win32_directx.h"


#if 0
static int 
D3D11RendererIsValid(d3d11_info *Renderer)
{
    int Result = (Renderer->Device &&
                  Renderer->SwapChain &&
                  //Renderer->ComputeShader &&
                  Renderer->ConstantBuffer);
    
    return Result;
}
#endif

static void
ReleaseD3D11Info(d3d11_info *Renderer)
{
    // TODO(Eric): Does releasing the Device release all it's subcomponents?
    
    //if(Renderer->ComputeShader) ID3D11ComputeShader_Release(Renderer->ComputeShader);
    
    //if(Renderer->PixelShader) ID3D11ComputeShader_Release(Renderer->PixelShader);
    //if(Renderer->VertexShader) ID3D11ComputeShader_Release(Renderer->VertexShader);
    
    //if(Renderer->ConstantBuffer) ID3D11Buffer_Release(Renderer->ConstantBuffer);
    
    if(Renderer->RenderTargetView) ID3D11UnorderedAccessView_Release(Renderer->RenderTargetView);
    if(Renderer->SwapChain) IDXGISwapChain2_Release(Renderer->SwapChain);
    
    if(Renderer->DeviceContext) ID3D11DeviceContext_Release(Renderer->DeviceContext);
    if(Renderer->DeviceContext1) ID3D11DeviceContext1_Release(Renderer->DeviceContext1);
    if(Renderer->Device) ID3D11Device_Release(Renderer->Device);
    
    d3d11_info ZeroRenderer = {0};
    *Renderer = ZeroRenderer;
}

static void 
ActivateD3D11DebugInfo(ID3D11Device *Device)
{
    ID3D11InfoQueue *Info;
    if(SUCCEEDED(IProvideClassInfo_QueryInterface(Device, &IID_ID3D11InfoQueue, (void**)&Info)))
    {
        HRESULT hr = ID3D11InfoQueue_SetBreakOnSeverity(Info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        AssertHR(hr);
        
        hr = ID3D11InfoQueue_SetBreakOnSeverity(Info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        AssertHR(hr);
        
        ID3D11InfoQueue_Release(Info);
    }
}

static IDXGIFactory2 *
AcquireDXGIFactory(ID3D11Device *Device)
{
    IDXGIFactory2 *Result = 0;
    
    if(Device)
    {
        IDXGIDevice *DxgiDevice = 0;
        if(SUCCEEDED(ID3D11Device_QueryInterface(Device, &IID_IDXGIDevice, (void **)&DxgiDevice)))
        {
            IDXGIAdapter *DxgiAdapter = 0;
            if(SUCCEEDED(IDXGIDevice_GetAdapter(DxgiDevice, &DxgiAdapter)))
            {
                IDXGIAdapter_GetParent(DxgiAdapter, &IID_IDXGIFactory2, (void**)&Result);
                
                IDXGIAdapter_Release(DxgiAdapter);
            }
            
            IDXGIDevice_Release(DxgiDevice);
        }
    }
    
    return Result;
}

static IDXGISwapChain2 *
AcquireDXGISwapChain(ID3D11Device *Device, HWND Window, int UseComputeShader)
{
    IDXGISwapChain2 *Result = 0;
    
    if(Device)
    {
        IDXGIFactory2 *DxgiFactory = AcquireDXGIFactory(Device);
        if(DxgiFactory)
        {
            DXGI_SWAP_CHAIN_DESC1 SwapChainDesc =
            {
                .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
                .SampleDesc = {1, 0},
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 2,
                .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                .Scaling = DXGI_SCALING_NONE,
                .AlphaMode = DXGI_ALPHA_MODE_IGNORE,
                .Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
            };
            
            if(UseComputeShader)
            {
                SwapChainDesc.BufferUsage |= DXGI_USAGE_UNORDERED_ACCESS;
            }
            
            IDXGISwapChain1 *SwapChain1 = 0;
            if(SUCCEEDED(IDXGIFactory2_CreateSwapChainForHwnd(DxgiFactory, (IUnknown*)Device, Window, &SwapChainDesc, NULL, NULL, &SwapChain1)))
            {
                if(SUCCEEDED(IDXGISwapChain1_QueryInterface(SwapChain1, &IID_IDXGISwapChain2, (void **)&Result)))
                {
                    IDXGIFactory2_MakeWindowAssociation(DxgiFactory, Window, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
                }
                
                IDXGISwapChain1_Release(SwapChain1);
            }
            
            IDXGIFactory2_Release(DxgiFactory);
        }
    }
    
    return Result;
}

static void
CheckMsaa(ID3D11Device *Device)
{
    // All D3D11 Capable devices support 4x MSAA with all render target formats (though differ quality levels)
    UINT MsaaQuality;
    HRESULT hr = ID3D11Device_CheckMultisampleQualityLevels(Device, DXGI_FORMAT_R8G8B8A8_UNORM, 4, &MsaaQuality);
    
    // Because 4x MSAA is always supported, the returned quality should always > 0
    Assert(SUCCEEDED(hr) && MsaaQuality > 0); 
}

static d3d11_info
AcquireD3D11Renderer(HWND Window, int EnableDebugging)
{
    // NOTE(Eric): Martins is awesome.
    // https://gist.github.com/mmozeiko/5e727f845db182d468a34d524508ad5f
    
    d3d11_info Result = {0};
    d3d11_info *d3d = &Result;
    
    UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;
    if(EnableDebugging)
    {
        Flags |= D3D11_CREATE_DEVICE_DEBUG;
    }
    
    D3D_FEATURE_LEVEL Levels[] = {D3D_FEATURE_LEVEL_11_0};
    
    DXGI_SWAP_CHAIN_DESC desc =
    {
        .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc = { 1, 0 },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .OutputWindow = Window,
        .Windowed = TRUE,
        
        // use more efficient flip model, available in Windows 10
        // if Windows 8 compatibility required, use DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
        // if Windows 7/Vista compatibility required, use DXGI_SWAP_EFFECT_DISCARD
        // NOTE: flip models do not allow MSAA framebuffer, so if you want MSAA then
        // you'll need to render offscreen and afterwards resolve to non-MSAA framebuffer
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
    };
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, Flags, Levels, ArrayCount(Levels),
                                               D3D11_SDK_VERSION, &desc, &Result.SwapChain, &Result.Device, NULL, &Result.DeviceContext);
    AssertHR(hr);
    
    ActivateD3D11DebugInfo(Result.Device);
    
    // disable stupid Alt+Enter changing monitor resolution to match window size
    {
        IDXGIFactory* factory;
        hr = IDXGISwapChain_GetParent(Result.SwapChain, &IID_IDXGIFactory, (void**)&factory);
        AssertHR(hr);
        IDXGIFactory_MakeWindowAssociation(factory, Window, DXGI_MWA_NO_ALT_ENTER);
        AssertHR(hr);
        IDXGIFactory_Release(factory);
    }
    
    // NOTE(Eric): Create Vertex Buffer and shaders was here
    
    // Create Texture View (tied to vertex shader, I think)
    {
        // checkerboard texture, with 50% transparency on black colors
        unsigned int pixels[] =
        {
            0x80000000, 0xffffffff,
            0xffffffff, 0x80000000,
        };
        UINT width = 2;
        UINT height = 2;
        
        D3D11_TEXTURE2D_DESC desc =
        {
            .Width = width,
            .Height = height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = { 1, 0 },
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_SHADER_RESOURCE,
        };
        
        D3D11_SUBRESOURCE_DATA data =
        {
            .pSysMem = pixels,
            .SysMemPitch = width * sizeof(unsigned int),
        };
        
        ID3D11Texture2D* texture;
        hr = ID3D11Device_CreateTexture2D(d3d->Device, &desc, &data, &texture);
        AssertHR(hr);
        
        hr = ID3D11Device_CreateShaderResourceView(d3d->Device, (ID3D11Resource*)texture, NULL, &d3d->TextureView);
        AssertHR(hr);
        
        ID3D11Texture2D_Release(texture);
    }
    
    // Create Sampler State
    {
        D3D11_SAMPLER_DESC desc =
        {
            .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
        };
        
        hr = ID3D11Device_CreateSamplerState(d3d->Device, &desc, &d3d->Sampler);
        AssertHR(hr);
    }
    
    // Create Blend State
    {
        // enable alpha blending
        D3D11_BLEND_DESC desc =
        {
            .RenderTarget[0] =
            {
                .BlendEnable = TRUE,
                .SrcBlend = D3D11_BLEND_SRC_ALPHA,
                .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
                .BlendOp = D3D11_BLEND_OP_ADD,
                .SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA,
                .DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
                .BlendOpAlpha = D3D11_BLEND_OP_ADD,
                .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
            },
        };
        hr = ID3D11Device_CreateBlendState(d3d->Device, &desc, &d3d->BlendState);
        AssertHR(hr);
    }
    
    // Create Rasterizer State
    {
        // disable culling
        D3D11_RASTERIZER_DESC desc =
        {
            .FillMode = D3D11_FILL_SOLID,
            .CullMode = D3D11_CULL_NONE,
        };
        hr = ID3D11Device_CreateRasterizerState(d3d->Device, &desc, &d3d->RasterizerState);
        AssertHR(hr);
    }
    
    // Create Depth State
    {
        D3D11_DEPTH_STENCIL_DESC desc =
        {
            .DepthEnable = TRUE,
            .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = D3D11_COMPARISON_LESS,
            .StencilEnable = TRUE,
            .StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
            
            // Stencil operations if pixel is front-facing
            .FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
            .FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR,
            .FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
            .FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
            
            // Stencil operations if pixel is back-facing
            .BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
            .BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR,
            .BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
            .BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
        };
        hr = ID3D11Device_CreateDepthStencilState(d3d->Device, &desc, &d3d->DepthState);
        AssertHR(hr);
    }
    
    if(!Result.SwapChain)
    {
        ReleaseD3D11Info(&Result);
    }
    
    return Result;
}
