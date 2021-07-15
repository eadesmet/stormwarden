
// NOTE(Eric): Ryan's "opengl.h" isn't specific to the platform, DirectX is windows only
#if BUILD_WIN32
#include <windows.h>
#include <d3d11_1.h>
#include <dxgi1_3.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")

#else
#error "DirectX11 not supported on this platform."
#endif



typedef struct d3d11_info d3d11_info;
struct d3d11_info
{
    ID3D11Device *Device;
    ID3D11DeviceContext *DeviceContext;
    ID3D11DeviceContext1 *DeviceContext1;
    
    IDXGISwapChain2 *SwapChain;
    HANDLE FrameLatencyWaitableObject;
    
    ID3D11ComputeShader *ComputeShader;
    ID3D11PixelShader *PixelShader;
    ID3D11VertexShader *VertexShader;
    
    ID3D11Buffer *ConstantBuffer;
    ID3D11RenderTargetView *RenderTarget;
    ID3D11UnorderedAccessView *RenderView;
};

