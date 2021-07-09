
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


// TODO(Eric): So I got this "working" (compiling, not used), but I really need to redo it
// BECAUSE at the moment I don't "get" it. It's a lot of code I didn't bother reading yet
// What I should do is delete it all and read through the code, and pull in what I need
// and get some simple shaders working. And remember to compile the shader in build.bat


// TODO(Eric): Remove this, it's specific to refterm
typedef struct
{
    uint32_t CellSize[2];
    uint32_t TermSize[2];
    uint32_t TopLeftMargin[2];
    uint32_t BlinkModulate;
    uint32_t MarginColor;
} renderer_const_buffer;

// TODO(Eric): Remove this, it's specific to refterm
#define RENDERER_CELL_BLINK 0x80000000
typedef struct
{
    uint32_t GlyphIndex;
    uint32_t Foreground;
    uint32_t Background; // NOTE(casey): The top bit of the background flag indicates blinking
} renderer_cell;


typedef struct d3d11_renderer d3d11_renderer;
struct d3d11_renderer
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
    
    ID3D11Buffer *CellBuffer;
    ID3D11ShaderResourceView *CellView;
    
    ID3D11Texture2D *GlyphTexture;
    ID3D11ShaderResourceView *GlyphTextureView;
    
    ID3D11Texture2D *GlyphTransfer;
    ID3D11ShaderResourceView *GlyphTransferView;
    IDXGISurface *GlyphTransferSurface;
    
    // NOTE(casey): These are for DirectWrite
    struct ID2D1RenderTarget *DWriteRenderTarget;
    struct ID2D1SolidColorBrush *DWriteFillBrush;
    
    uint32_t CurrentWidth;
    uint32_t CurrentHeight;
    uint32_t MaxCellCount;
    
    int UseComputeShader;
};
