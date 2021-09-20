
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
    
    // NOTE(Eric): I feel like this group is all tied closely together.
    // So the question becomes: Do we need all of these things for each different type of 'object' we want to render?
    ID3D11InputLayout *InputLayout;
    
    // NOTE(Eric): Testing drawing multiple things
    //ID3D11Buffer *VertexBuffer;
    ID3D11Buffer *VertexBuffer[16];
    u16 VertexBufferCount;
    
    ID3D11VertexShader *VertexShader[16];
    u16 VertexShaderCount;
    
    ID3D11PixelShader *PixelShader[16];
    u16 PixelShaderCount;
    
    ID3D11SamplerState* Sampler;
    ID3D11RasterizerState* RasterizerState;
    ID3D11BlendState* BlendState;
    ID3D11DepthStencilState* DepthState;
    ID3D11ShaderResourceView* TextureView;
    ID3D11Buffer *ConstantBuffer;
    
    ID3D11RenderTargetView *RenderTargetView;
    ID3D11DepthStencilView *DepthStencilView;
};
