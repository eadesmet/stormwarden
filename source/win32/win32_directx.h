
// NOTE(Eric): Ryan's "opengl.h" isn't specific to the platform, DirectX is windows only
#if BUILD_WIN32

#define COBJMACROS
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
    
    IDXGISwapChain *SwapChain;
    
    ID3D11SamplerState* Sampler;
    ID3D11RasterizerState* RasterizerState;
    ID3D11BlendState* BlendState;
    ID3D11DepthStencilState* DepthState;
    ID3D11ShaderResourceView* TextureView;
    
    ID3D11RenderTargetView *RenderTargetView;
    ID3D11DepthStencilView *DepthStencilView;
};


typedef struct render_info render_info;
struct render_info
{
    ID3D11InputLayout *InputLayout;
    ID3D11Buffer *VertexBuffer;
    ID3D11Buffer *IndexBuffer;
    ID3D11VertexShader *VertexShader;
    ID3D11PixelShader *PixelShader;
    ID3D11Buffer *ConstantBuffer;
};

typedef u32 render_info_type;
enum
{
    RenderInfoType_SampleTriangle = (1<<0),
    RenderInfoType_Square         = (1<<1),
    
    
    RenderInfoType_Count          = (1<<2)
};