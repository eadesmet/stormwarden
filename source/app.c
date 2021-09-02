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

#include "entity.h"

global LARGE_INTEGER G_Freq, G_c1;

APP_PERMANENT_LOAD// NOTE(Eric): INIT
{
    os = os_;
    d3d11_info *d3d = &os->d3d;
    HRESULT hr;
    
    // TODO(Eric): Replace this with our own Timer, based on these calls and luna's timer
    QueryPerformanceFrequency(&G_Freq);
    QueryPerformanceCounter(&G_c1);
    
    // NOTE(Eric): Create vertex buffer
    {
        vertex_data data[] =
        {
            { { -0.00f, +0.75f }, { 25.0f, 50.0f }, { 1, 0, 0 } },
            { { +0.75f, -0.50f }, {  0.0f,  0.0f }, { 0, 1, 0 } },
            { { -0.75f, -0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } },
        };
        
        D3D11_BUFFER_DESC desc =
        {
            .ByteWidth = sizeof(data),
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };
        
        D3D11_SUBRESOURCE_DATA initial = { .pSysMem = data };
        hr = ID3D11Device_CreateBuffer(d3d->Device, &desc, &initial, &d3d->VertexBuffer[d3d->VertexBufferCount]);
        AssertHR(hr);

        d3d->VertexBufferCount++;
    }

    // NOTE(Eric): Attempting to draw another thing.
    {
        vertex_data data[] =
        {
            // Position, UV, Color
            { { -1.00f, -1.00f }, { 25.0f, 50.0f }, { 1, 0, 0 } }, // Top-left?
            { { +0.50f, -0.50f }, {  0.0f,  0.0f }, { 0, 1, 0 } }, // Center?
            { { -1.00f, -0.00f }, { 50.0f,  0.0f }, { 0, 0, 1 } }, // Center-Left?
        };

        D3D11_BUFFER_DESC desc =
        {
            .ByteWidth = sizeof(data),
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };

        D3D11_SUBRESOURCE_DATA initial = { .pSysMem = data };
        hr = ID3D11Device_CreateBuffer(d3d->Device, &desc, &initial, &d3d->VertexBuffer[d3d->VertexBufferCount]);
        AssertHR(hr);

        d3d->VertexBufferCount++;
    }
    
    // NOTE(Eric): Set InputLayout and create shaders
    {
        // these must match vertex shader input layout
        D3D11_INPUT_ELEMENT_DESC desc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(struct vertex_data, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(struct vertex_data, UV),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(struct vertex_data, Color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        
        // NOTE(Eric): Compiled shader files with fxc.exe
#include "shaders/d3d11_vshader.h"
        hr = ID3D11Device_CreateVertexShader(d3d->Device, d3d11_vshader, sizeof(d3d11_vshader), NULL, &d3d->VertexShader[d3d->VertexShaderCount++]);
        AssertHR(hr);
        
#include "shaders/d3d11_pshader.h"
        hr = ID3D11Device_CreatePixelShader(d3d->Device, d3d11_pshader, sizeof(d3d11_pshader), NULL, &d3d->PixelShader[d3d->PixelShaderCount++]);
        AssertHR(hr);
        
        hr = ID3D11Device_CreateInputLayout(d3d->Device, desc, _countof(desc), d3d11_vshader, sizeof(d3d11_vshader), &d3d->InputLayout);
        AssertHR(hr);



        // NOTE(Eric): Try to compile my own shaders
#include "shaders/d3d11_eric_vshader.h"
        hr = ID3D11Device_CreateVertexShader(d3d->Device, d3d11_eric_vshader, sizeof(d3d11_eric_vshader), NULL, &d3d->VertexShader[d3d->VertexShaderCount++]);
        AssertHR(hr);

#include "shaders/d3d11_eric_pshader.h"
        hr = ID3D11Device_CreatePixelShader(d3d->Device, d3d11_eric_pshader, sizeof(d3d11_eric_pshader), NULL, &d3d->PixelShader[d3d->PixelShaderCount++]);
        AssertHR(hr);

    }
    
    
    // Create Constant Buffer, used only for the Transform matrix multiply
    {
        D3D11_BUFFER_DESC desc =
        {
            .ByteWidth = 2 * 4 * sizeof(float),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };
        hr = ID3D11Device_CreateBuffer(d3d->Device, &desc, NULL, &d3d->ConstantBuffer);
        AssertHR(hr);
    }
    
    
}

APP_HOT_LOAD// NOTE(Eric): INIT AND ON CODE-RELOAD
{
    os = os_;
}

APP_HOT_UNLOAD {}

APP_UPDATE// NOTE(Eric): PER FRAME
{
    d3d11_info *d3d = &os->d3d;
    
    HRESULT hr;
    int width = os->window_size.width;
    int height = os->window_size.height;
    
    if (d3d->RenderTargetView == NULL || os->resized)
    {
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
    
    if (d3d->RenderTargetView)
    {
        LARGE_INTEGER c2;
        QueryPerformanceCounter(&c2);
        f32 delta = (float)((double)(c2.QuadPart - G_c1.QuadPart) / G_Freq.QuadPart);
        G_c1 = c2;
        
        // output viewport covering all client area of window
        D3D11_VIEWPORT viewport =
        {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = (FLOAT)os->window_size.width,
            .Height = (FLOAT)os->window_size.height,
            .MinDepth = 0,
            .MaxDepth = 1,
        };
        
        FLOAT ClearColor[] = { 0.392f, 0.584f, 0.929f, 1.f };
        ID3D11DeviceContext_ClearRenderTargetView(d3d->DeviceContext, d3d->RenderTargetView, ClearColor);
        ID3D11DeviceContext_ClearDepthStencilView(d3d->DeviceContext, d3d->DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
        
        // setup rotation matrix in uniform
        global f32 angle = 0;
        {
            angle += delta * 2.0f * (float)PI / 20.0f; // full rotation in 20 seconds
            angle = fmodf(angle, 2.0f * (float)PI);
            
            float aspect = (float)height / width;
            float matrix[] =
            {
                Cos(angle) * aspect, -Sin(angle), 0.f, 0.f,
                Sin(angle) * aspect,  Cos(angle), 0.f, 0.f,
            };
            
            D3D11_MAPPED_SUBRESOURCE mapped;
            hr = ID3D11DeviceContext_Map(d3d->DeviceContext, (ID3D11Resource*)d3d->ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            AssertHR(hr);
            memcpy(mapped.pData, matrix, sizeof(matrix));
            ID3D11DeviceContext_Unmap(d3d->DeviceContext, (ID3D11Resource*)d3d->ConstantBuffer, 0);
        }
        
        // NOTE(Eric): Render to screen
        {
            // Input Assembler
            ID3D11DeviceContext_IASetInputLayout(d3d->DeviceContext, d3d->InputLayout);
            ID3D11DeviceContext_IASetPrimitiveTopology(d3d->DeviceContext, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            UINT stride = sizeof(vertex_data);
            UINT offset = 0;
            ID3D11DeviceContext_IASetVertexBuffers(d3d->DeviceContext, 0, 1, &d3d->VertexBuffer[0], &stride, &offset);
            
            // Vertex Shader
            ID3D11DeviceContext_VSSetConstantBuffers(d3d->DeviceContext, 0, 1, &d3d->ConstantBuffer);
            ID3D11DeviceContext_VSSetShader(d3d->DeviceContext, d3d->VertexShader[0], NULL, 0);
            
            // Rasterizer Stage
            ID3D11DeviceContext_RSSetViewports(d3d->DeviceContext, 1, &viewport);
            ID3D11DeviceContext_RSSetState(d3d->DeviceContext, d3d->RasterizerState);

            // Pixel Shader
            ID3D11DeviceContext_PSSetSamplers(d3d->DeviceContext, 0, 1, &d3d->Sampler);
            ID3D11DeviceContext_PSSetShaderResources(d3d->DeviceContext, 0, 1, &d3d->TextureView);
            ID3D11DeviceContext_PSSetShader(d3d->DeviceContext, d3d->PixelShader[0], NULL, 0);
            
            // Output Merger
            ID3D11DeviceContext_OMSetBlendState(d3d->DeviceContext, d3d->BlendState, NULL, ~0U);
            ID3D11DeviceContext_OMSetDepthStencilState(d3d->DeviceContext, d3d->DepthState, 0);
            ID3D11DeviceContext_OMSetRenderTargets(d3d->DeviceContext, 1, &d3d->RenderTargetView, d3d->DepthStencilView);
            
            // draw 3 vertices
            ID3D11DeviceContext_Draw(d3d->DeviceContext, 3, 0);

            {
                // NOTE(Eric): Try to draw another thing. Sweet, this worked. But are all of these steps necessary?
                // Input Assembler
                ID3D11DeviceContext_IASetInputLayout(d3d->DeviceContext, d3d->InputLayout);
                ID3D11DeviceContext_IASetPrimitiveTopology(d3d->DeviceContext, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                UINT stride = sizeof(vertex_data);
                UINT offset = 0;
                ID3D11DeviceContext_IASetVertexBuffers(d3d->DeviceContext, 0, 1, &d3d->VertexBuffer[1], &stride, &offset);

                // Vertex Shader
                //ID3D11DeviceContext_VSSetConstantBuffers(d3d->DeviceContext, 0, 1, &d3d->ConstantBuffer);
                ID3D11DeviceContext_VSSetShader(d3d->DeviceContext, d3d->VertexShader[1], NULL, 0);

                // Rasterizer Stage
                ID3D11DeviceContext_RSSetViewports(d3d->DeviceContext, 1, &viewport);
                ID3D11DeviceContext_RSSetState(d3d->DeviceContext, d3d->RasterizerState);

                // Pixel Shader
                //ID3D11DeviceContext_PSSetSamplers(d3d->DeviceContext, 0, 1, &d3d->Sampler);
                //ID3D11DeviceContext_PSSetShaderResources(d3d->DeviceContext, 0, 1, &d3d->TextureView);
                ID3D11DeviceContext_PSSetShader(d3d->DeviceContext, d3d->PixelShader[1], NULL, 0);

                // Output Merger
                ID3D11DeviceContext_OMSetBlendState(d3d->DeviceContext, d3d->BlendState, NULL, ~0U);
                ID3D11DeviceContext_OMSetDepthStencilState(d3d->DeviceContext, d3d->DepthState, 0);
                ID3D11DeviceContext_OMSetRenderTargets(d3d->DeviceContext, 1, &d3d->RenderTargetView, d3d->DepthStencilView);

                // draw 3 vertices
                ID3D11DeviceContext_Draw(d3d->DeviceContext, 3, 0);
            }
            
            
            hr = IDXGISwapChain_Present(d3d->SwapChain, os->vsync, 0);
            if (hr == DXGI_STATUS_OCCLUDED)
            {
                // window is minimized, cannot vsync - instead sleep a bit
                if (os->vsync)
                {
                    Sleep(10);
                }
            }
            else if (FAILED(hr))
            {
                LogError("Failed to present swap chain! Device lost?");
            }
            
        }
    }
    
    //os->RefreshScreen();
}










// NOTE(Eric): Alright, so drawing multiple things goes like this:
/*
// Draw cubes
{
    d3d11DeviceContext->IASetInputLayout(blinnPhongInputLayout);
    d3d11DeviceContext->VSSetShader(blinnPhongVertexShader, nullptr, 0);
    d3d11DeviceContext->PSSetShader(blinnPhongPixelShader, nullptr, 0);

    d3d11DeviceContext->PSSetShaderResources(0, 1, &textureView);
    d3d11DeviceContext->PSSetSamplers(0, 1, &samplerState);

    d3d11DeviceContext->VSSetConstantBuffers(0, 1, &blinnPhongVSConstantBuffer);
    d3d11DeviceContext->PSSetConstantBuffers(0, 1, &blinnPhongPSConstantBuffer);

    // Update pixel shader constant buffer
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        d3d11DeviceContext->Map(blinnPhongPSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
        BlinnPhongPSConstants* constants = (BlinnPhongPSConstants*)(mappedSubresource.pData);
        constants->dirLight.dirEye = normalise(float4{1.f, 1.f, 1.f, 0.f});
        constants->dirLight.color = {0.7f, 0.8f, 0.2f, 1.f};
        for(int i=0; i<NUM_LIGHTS; ++i){
            constants->pointLights[i].posEye = pointLightPosEye[i];
            constants->pointLights[i].color = lightColor[i];
        }
        d3d11DeviceContext->Unmap(blinnPhongPSConstantBuffer, 0);
    }

    for(int i=0; i<NUM_CUBES; ++i)
    {
        // Update vertex shader constant buffer
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        d3d11DeviceContext->Map(blinnPhongVSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
        BlinnPhongVSConstants* constants = (BlinnPhongVSConstants*)(mappedSubresource.pData);
        constants->modelViewProj = cubeModelViewMats[i] * perspectiveMat;
        constants->modelView = cubeModelViewMats[i];
        constants->normalMatrix = cubeNormalMats[i];
        d3d11DeviceContext->Unmap(blinnPhongVSConstantBuffer, 0);

        d3d11DeviceContext->DrawIndexed(cubeNumIndices, 0, 0);
    }
}


*/