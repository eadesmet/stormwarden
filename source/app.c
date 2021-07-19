#include "language_layer.h"
#include "maths.h"
#include "memory.h"
#include "strings.h"
#include "perlin.h"
#include "win32/win32_directx.c" // NOTE(Eric): Whew, this is rough, I need to figure out what to do with this
#include "os.h"
#include "opengl.h"

#include "language_layer.c"
#include "maths.c"
#include "memory.c"
#include "strings.c"
#include "perlin.c"
#include "os.c"

// NOTE(Eric): Getting into DirectX now instead, going through the Luna book
//#include "gl_arcsynth.h"
//#include "gl_arcsynth.c"


APP_PERMANENT_LOAD// NOTE(Eric): INIT
{
    os = os_;
    
}

APP_HOT_LOAD// NOTE(Eric): INIT AND ON CODE-RELOAD
{
    os = os_;
}

APP_HOT_UNLOAD {}

APP_UPDATE// NOTE(Eric): PER FRAME
{
    d3d11_info *d3d = &os->d3d;
    if (d3d->RenderTargetView == NULL || os->resized)
    {
        // TODO(Eric): RESET VIEWPORT
        //glViewport(0, 0, (GLsizei)os->window_size.width, (GLsizei)os->window_size.height);
        
        if (d3d->RenderTargetView)
        {
            // release old swap chain buffers
            ID3D11DeviceContext_ClearState(d3d->DeviceContext);
            ID3D11RenderTargetView_Release(d3d->RenderTargetView);
            ID3D11DepthStencilView_Release(d3d->DepthStencilView);
            d3d->RenderTargetView = NULL;
        }
        
        if (os->window_size.width != 0 && os->window_size.height != 0)
        {
            int width = os->window_size.width;
            int height = os->window_size.height;
            
            // NOTE(Eric): Resize
            HRESULT hr = IDXGISwapChain_ResizeBuffers(d3d->SwapChain, 0, width, height, DXGI_FORMAT_UNKNOWN, 0);
            if (FAILED(hr))
            {
                LogError("Failed to resize swap chain!");
            }
            
            D3D11_RENDER_TARGET_VIEW_DESC rtDesc =
            {
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM, // or use DXGI_FORMAT_R8G8B8A8_UNORM_SRGB for storing sRGB
                .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
            };
            
            // create RenderTarget view for new backbuffer texture
            ID3D11Texture2D* backbuffer;
            hr = IDXGISwapChain_GetBuffer(d3d->SwapChain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
            AssertHR(hr);
            hr = ID3D11Device_CreateRenderTargetView(d3d->Device, (ID3D11Resource*)backbuffer, &rtDesc, &d3d->RenderTargetView);
            AssertHR(hr);
            ID3D11Texture2D_Release(backbuffer);
            
            D3D11_TEXTURE2D_DESC depthDesc =
            {
                .Width = width,
                .Height = height,
                .MipLevels = 1,
                .ArraySize = 1,
                .Format = DXGI_FORMAT_D24_UNORM_S8_UINT, // or use DXGI_FORMAT_D32_FLOAT if you don't need stencil
                .SampleDesc = { 1, 0 },
                .Usage = D3D11_USAGE_DEFAULT,
                .BindFlags = D3D11_BIND_DEPTH_STENCIL,
            };
            
            // create new depth stencil texture & DepthStencil view
            ID3D11Texture2D* depth;
            hr = ID3D11Device_CreateTexture2D(d3d->Device, &depthDesc, NULL, &depth);
            AssertHR(hr);
            hr = ID3D11Device_CreateDepthStencilView(d3d->Device, (ID3D11Resource*)depth, NULL, &d3d->DepthStencilView);
            AssertHR(hr);
            ID3D11Texture2D_Release(depth);
        }
        
        
        os->resized = 0;
    }
    
    if (os->event_count > 0)
    {
        // NOTE(Eric): Not sure if this is how I'm supposed to handle events in the app,
        // but it's working for now.
        for (u32 EventIndex = 0;
             EventIndex < os->event_count;
             EventIndex++)
        {
            OS_Event *Event = 0;
            OS_GetNextEvent(&Event);
            if(!Event) break;
            
            if (Event->type == OS_EventType_KeyPress)
            {
                switch(Event->key)
                {
                    case Key_Space:
                    {
                        
                    }break;
                    case Key_Esc:
                    {
                        os->quit = 1;
                    }break;
                    default:break;
                }
            }
            
            OS_EatEvent(Event);
        }
    }
    
    FLOAT ClearColor[] = { 0.392f, 0.584f, 0.929f, 1.f };
    ID3D11DeviceContext_ClearRenderTargetView(d3d->DeviceContext, d3d->RenderTargetView, ClearColor);
    //ID3D11DeviceContext_Draw(os->d3d.DeviceContext, 4, 0);
    
    HRESULT hr = IDXGISwapChain1_Present(os->d3d.SwapChain, 0, 0);
    
    //os->RefreshScreen();
}
