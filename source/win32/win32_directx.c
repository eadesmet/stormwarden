
// TODO(Eric): 7/7/2021: Look at refterm as a reference for getting directx up and running in C
// and apply it here. 
// - Have the ability to switch between directx and opengl.
// - make a generic 'renderer', basically with an if(opengl) {use opengl} else if (directx) {use directx}
//   - Though, why would I need this?
// - Go through the directx luna book and _make some progress!_

#include "win32_directx.h"


static int D3D11RendererIsValid(d3d11_renderer *Renderer)
{
    int Result = (Renderer->Device &&
                  Renderer->SwapChain &&
                  Renderer->ComputeShader &&
                  Renderer->ConstantBuffer &&
                  Renderer->CellView &&
                  Renderer->GlyphTextureView);
    
    return Result;
}

static void ActivateD3D11DebugInfo(ID3D11Device *Device)
{
    ID3D11InfoQueue *Info;
    if(SUCCEEDED(IProvideClassInfo_QueryInterface(Device, &IID_ID3D11InfoQueue, (void**)&Info)))
    {
        ID3D11InfoQueue_SetBreakOnSeverity(Info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(Info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        
        ID3D11InfoQueue_Release(Info);
    }
}

static IDXGIFactory2 *AcquireDXGIFactory(ID3D11Device *Device)
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

static IDXGISwapChain2 *AcquireDXGISwapChain(ID3D11Device *Device, HWND Window, int UseComputeShader)
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


static void ReleaseD3DCellBuffer(d3d11_renderer *Renderer)
{
    if(Renderer->CellBuffer)
    {
        ID3D11Buffer_Release(Renderer->CellBuffer);
        Renderer->CellBuffer = 0;
    }
    
    if(Renderer->CellView)
    {
        ID3D11ShaderResourceView_Release(Renderer->CellView);
        Renderer->CellView = 0;
    }
}

static void ClearD3D11GlyphTexture(d3d11_renderer *Renderer)
{
    if(Renderer->GlyphTextureView)
    {
        FLOAT Zero[4] = {0};
        ID3D11DeviceContext1_ClearView(Renderer->DeviceContext1, (ID3D11View *)Renderer->GlyphTextureView, Zero, 0, 0);
    }
}

static void SetD3D11MaxCellCount(d3d11_renderer *Renderer, uint32_t Count)
{
    ReleaseD3DCellBuffer(Renderer);
    
    if(Renderer->Device)
    {
        D3D11_BUFFER_DESC CellBufferDesc =
        {
            .ByteWidth = Count * sizeof(renderer_cell),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_SHADER_RESOURCE,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
            .MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
            .StructureByteStride = sizeof(renderer_cell),
        };
        
        if(SUCCEEDED(ID3D11Device_CreateBuffer(Renderer->Device, &CellBufferDesc, 0, &Renderer->CellBuffer)))
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC CellViewDesc =
            {
                .ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
                .Buffer.FirstElement = 0,
                .Buffer.NumElements = Count,
            };
            
            ID3D11Device_CreateShaderResourceView(Renderer->Device, (ID3D11Resource *)Renderer->CellBuffer, &CellViewDesc, &Renderer->CellView);
        }
        
        Renderer->MaxCellCount = Count;
    }
}

static void ReleaseD3DGlyphCache(d3d11_renderer *Renderer)
{
    if(Renderer->GlyphTexture)
    {
        ID3D11ShaderResourceView_Release(Renderer->GlyphTexture);
        Renderer->GlyphTexture = 0;
    }
    
    if(Renderer->GlyphTextureView)
    {
        ID3D11ShaderResourceView_Release(Renderer->GlyphTextureView);
        Renderer->GlyphTextureView = 0;
    }
}

static void ReleaseD3DGlyphTransfer(d3d11_renderer *Renderer)
{
    //D2DRelease(&Renderer->DWriteRenderTarget, &Renderer->DWriteFillBrush);
    
    if(Renderer->GlyphTransfer)
    {
        ID3D11ShaderResourceView_Release(Renderer->GlyphTransfer);
        Renderer->GlyphTransfer = 0;
    }
    
    if(Renderer->GlyphTransferView)
    {
        ID3D11ShaderResourceView_Release(Renderer->GlyphTransferView);
        Renderer->GlyphTransferView = 0;
    }
    
    if(Renderer->GlyphTransferSurface)
    {
        IDXGISurface_Release(Renderer->GlyphTransferSurface);
        Renderer->GlyphTransferSurface = 0;
    }
}

static void SetD3D11GlyphCacheDim(d3d11_renderer *Renderer, uint32_t Width, uint32_t Height)
{
    ReleaseD3DGlyphCache(Renderer);
    
    if(Renderer->Device)
    {
        D3D11_TEXTURE2D_DESC TextureDesc =
        {
            .Width = Width,
            .Height = Height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
            .SampleDesc = { 1, 0 },
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_SHADER_RESOURCE,
        };
        
        if(SUCCEEDED(ID3D11Device_CreateTexture2D(Renderer->Device, &TextureDesc, NULL, &Renderer->GlyphTexture)))
        {
            ID3D11Device_CreateShaderResourceView(Renderer->Device, (ID3D11Resource*)Renderer->GlyphTexture, NULL, &Renderer->GlyphTextureView);
        }
    }
}

static void SetD3D11GlyphTransferDim(d3d11_renderer *Renderer, uint32_t Width, uint32_t Height)
{
    ReleaseD3DGlyphTransfer(Renderer);
    
    if(Renderer->Device)
    {
        D3D11_TEXTURE2D_DESC TextureDesc =
        {
            .Width = Width,
            .Height = Height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
            .SampleDesc = { 1, 0 },
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET,
        };
        
        if(SUCCEEDED(ID3D11Device_CreateTexture2D(Renderer->Device, &TextureDesc, 0, &Renderer->GlyphTransfer)))
        {
            ID3D11Device_CreateShaderResourceView(Renderer->Device, (ID3D11Resource *)Renderer->GlyphTransfer, 0, &Renderer->GlyphTransferView);
            ID3D11Texture2D_QueryInterface(Renderer->GlyphTransfer, &IID_IDXGISurface, (void **)&Renderer->GlyphTransferSurface);
            
            D2DAcquire(Renderer->GlyphTransferSurface,
                       &Renderer->DWriteRenderTarget,
                       &Renderer->DWriteFillBrush);
        }
    }
}


static void ReleaseD3D11RenderTargets(d3d11_renderer *Renderer)
{
    if (Renderer->RenderView)
    {
        ID3D11UnorderedAccessView_Release(Renderer->RenderView);
        Renderer->RenderView = 0;
    }
    
    if (Renderer->RenderTarget)
    {
        ID3D11RenderTargetView_Release(Renderer->RenderTarget);
        Renderer->RenderTarget = 0;
    }
}

static void ReleaseD3D11Renderer(d3d11_renderer *Renderer)
{
    // TODO(casey): When you want to release a D3D11 device, do you have to release all the sub-components?
    // Can you just release the main device and have all the sub-components release themselves?
    
    ReleaseD3DCellBuffer(Renderer);
    ReleaseD3DGlyphCache(Renderer);
    ReleaseD3DGlyphTransfer(Renderer);
    ReleaseD3D11RenderTargets(Renderer);
    
    if(Renderer->ComputeShader) ID3D11ComputeShader_Release(Renderer->ComputeShader);
    if(Renderer->PixelShader) ID3D11ComputeShader_Release(Renderer->PixelShader);
    if(Renderer->VertexShader) ID3D11ComputeShader_Release(Renderer->VertexShader);
    
    if(Renderer->ConstantBuffer) ID3D11Buffer_Release(Renderer->ConstantBuffer);
    
    if(Renderer->RenderView) ID3D11UnorderedAccessView_Release(Renderer->RenderView);
    if(Renderer->SwapChain) IDXGISwapChain2_Release(Renderer->SwapChain);
    
    if(Renderer->DeviceContext) ID3D11DeviceContext_Release(Renderer->DeviceContext);
    if(Renderer->DeviceContext1) ID3D11DeviceContext1_Release(Renderer->DeviceContext1);
    if(Renderer->Device) ID3D11Device_Release(Renderer->Device);
    
    d3d11_renderer ZeroRenderer = {0};
    *Renderer = ZeroRenderer;
}

static d3d11_renderer 
AcquireD3D11Renderer(HWND Window, int EnableDebugging)
{
    d3d11_renderer Result = {0};
    
    UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;
    if(EnableDebugging)
    {
        Flags |= D3D11_CREATE_DEVICE_DEBUG;
    }
    
    D3D_FEATURE_LEVEL Levels[] = {D3D_FEATURE_LEVEL_11_0};
    HRESULT hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, Flags, Levels, ARRAYSIZE(Levels), D3D11_SDK_VERSION,
                                   &Result.Device, 0, &Result.DeviceContext);
    if(FAILED(hr))
    {
        hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_WARP, 0, Flags, Levels, ARRAYSIZE(Levels), D3D11_SDK_VERSION,
                               &Result.Device, 0, &Result.DeviceContext);
    }
    
    if(SUCCEEDED(hr))
    {
        if(SUCCEEDED(ID3D11DeviceContext1_QueryInterface(Result.DeviceContext, &IID_ID3D11DeviceContext1, (void **)&Result.DeviceContext1)))
        {
            if(EnableDebugging)
            {
                ActivateD3D11DebugInfo(Result.Device);
            }
            
            Result.SwapChain = AcquireDXGISwapChain(Result.Device, Window, 0);
#if 0
            if(Result.SwapChain)
            {
                Result.FrameLatencyWaitableObject = IDXGISwapChain2_GetFrameLatencyWaitableObject(Result.SwapChain);
                
                D3D11_BUFFER_DESC ConstantBufferDesc =
                {
                    .ByteWidth = sizeof(renderer_const_buffer),
                    .Usage = D3D11_USAGE_DYNAMIC,
                    .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
                    .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
                };
                ID3D11Device_CreateBuffer(Result.Device, &ConstantBufferDesc, 0, &Result.ConstantBuffer);
                
                ID3D11Device_CreateComputeShader(Result.Device, ReftermCSShaderBytes, sizeof(ReftermCSShaderBytes), 0, &Result.ComputeShader);
                ID3D11Device_CreatePixelShader(Result.Device, ReftermPSShaderBytes, sizeof(ReftermPSShaderBytes), 0, &Result.PixelShader);
                ID3D11Device_CreateVertexShader(Result.Device, ReftermVSShaderBytes, sizeof(ReftermVSShaderBytes), 0, &Result.VertexShader);
            }
#endif
        }
    }
    
    if(!Result.SwapChain)
    {
        ReleaseD3D11Renderer(&Result);
    }
    
    return Result;
}
