
// NOTE(Eric): Ryan's "opengl.h" isn't specific to the platform, DirectX is windows only
#if BUILD_WIN32
#include <windows.h>
#include <dxgi.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
//#include <d3dcommon.h>

// For Factory2 ???????
#include <dxgi1_2.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#else
#error "DirectX11 not supported on this platform."
#endif

enum V_SHADER
{
    V_SHADER_STATIC, // Non-moveable thing
    V_SHADER_TEXTURE,
    
    V_SHADER_COUNT
};
enum P_SHADER
{
    P_SHADER_STATIC,
    P_SHADER_TEXTURE,
    
    P_SHADER_COUNT
};

typedef struct DX_Shaders DX_Shaders;
struct DX_Shaders
{
    ID3D11VertexShader* VertexShader[V_SHADER_COUNT];
    ID3D11InputLayout* InputLayout[V_SHADER_COUNT];
    
    ID3D11PixelShader* PixelShader[P_SHADER_COUNT];
};

typedef struct DX_State DX_State;
struct DX_State
{
    ID3D11Device1* Device;
    ID3D11DeviceContext1* DeviceContext;
    
    IDXGISwapChain1* SwapChain;
    
    ID3D11RenderTargetView* FrameBufferView;
    ID3D11DepthStencilView* DepthBufferView;
    
    DX_Shaders Shaders;
};

