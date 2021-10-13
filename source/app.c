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

#include "entity.c"
#include "render.c"

global LARGE_INTEGER G_Freq, G_c1;

APP_PERMANENT_LOAD// NOTE(Eric): INIT
{
    os = os_;
    d3d11_info *d3d = &os->d3d;
    game_state *GameState = (game_state *)M_ArenaPush(&os->permanent_arena, sizeof(game_state));
    
    f32 AspectRatio = (f32)((f32)os->window_size.width / (f32)os->window_size.height);
    GameState->Camera = CreateCamera(86.0f, AspectRatio);
    
    
    // TODO(Eric): Replace this with our own Timer, based on these calls and luna's timer
    QueryPerformanceFrequency(&G_Freq);
    QueryPerformanceCounter(&G_c1);
    
    InitRenderer(d3d, GameState);
    
    
    //~ NOTE(Eric): Init many squares?
    for (u32 Index = 0; Index < 16; Index++)
    {
        //entity_square *NewSquare = &GameState->Squares[Index];
        
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
    game_state *GameState = (game_state *)os->permanent_arena.base;
    
    HRESULT hr;
    int width = os->window_size.width;
    int height = os->window_size.height;
    
    // NOTE(Eric): Get the DeltaTime
    LARGE_INTEGER c2;
    QueryPerformanceCounter(&c2);
    f32 DeltaTime = (float)((double)(c2.QuadPart - G_c1.QuadPart) / G_Freq.QuadPart);
    G_c1 = c2;
    
    // NOTE(Eric): Used for sample triangle 
    global f32 angle = 0;
    
    // NOTE(Eric): Camera
    game_camera *Camera = &GameState->Camera;
    f32 CameraSpeed = 5.0f; // Meters per second
    f32 CameraMoveAmount = CameraSpeed * DeltaTime;
    
    
    if (d3d->RenderTargetView == NULL || os->resized)
    {
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
                    case Key_W:
                    {
                        
                    }break;
                    case Key_A:
                    {
                        
                    }break;
                    case Key_S:
                    {
                        
                    }break;
                    case Key_D:
                    {
                        
                    }break;
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
        // output viewport covering all client area of window
        D3D11_VIEWPORT WindowViewport =
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
        
        
        
        
        
        //
        //~ NOTE(Eric): Render to screen
        //~
        {
            // NOTE(Eric): Common renderer things that are either shared, or in d3d_info instead of render_info
            // Rasterizer Stage
            ID3D11DeviceContext_RSSetViewports(d3d->DeviceContext, 1, &WindowViewport);
            ID3D11DeviceContext_RSSetState(d3d->DeviceContext, d3d->RasterizerState);
            
            // Pixel Shader
            ID3D11DeviceContext_PSSetSamplers(d3d->DeviceContext, 0, 1, &d3d->Sampler);
            ID3D11DeviceContext_PSSetShaderResources(d3d->DeviceContext, 0, 1, &d3d->TextureView);
            
            // Output Merger
            ID3D11DeviceContext_OMSetBlendState(d3d->DeviceContext, d3d->BlendState, NULL, ~0U);
            ID3D11DeviceContext_OMSetDepthStencilState(d3d->DeviceContext, d3d->DepthState, 0);
            ID3D11DeviceContext_OMSetRenderTargets(d3d->DeviceContext, 1, &d3d->RenderTargetView, d3d->DepthStencilView);
            
            for (u32 RenderInfoTypeIndex = 0; RenderInfoTypeIndex < RenderInfoType_Count; RenderInfoTypeIndex++)
            {
                switch(RenderInfoTypeIndex)
                {
#if 0 // Don't draw the traingle for now.
                    case RenderInfoType_SampleTriangle:
                    {
                        render_info SampleTriangleInfo = GameState->RenderInfos[RenderInfoType_SampleTriangle];
                        
                        // Update constant buffer (setup rotation matrix in uniform)
                        angle += DeltaTime * 2.0f * (float)PI / 20.0f; // full rotation in 20 seconds
                        angle = fmodf(angle, 2.0f * (float)PI);
                        
                        float aspect = (float)height / width;
                        float matrix[] =
                        {
                            Cos(angle) * aspect, -Sin(angle), 0.f, 0.f,
                            Sin(angle) * aspect,  Cos(angle), 0.f, 0.f,
                        };
                        
                        D3D11_MAPPED_SUBRESOURCE mapped;
                        hr = ID3D11DeviceContext_Map(d3d->DeviceContext, (ID3D11Resource*)SampleTriangleInfo.ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
                        AssertHR(hr);
                        memcpy(mapped.pData, matrix, sizeof(matrix));
                        ID3D11DeviceContext_Unmap(d3d->DeviceContext, (ID3D11Resource*)SampleTriangleInfo.ConstantBuffer, 0);
                        
                        
                        // Input Assembler
                        ID3D11DeviceContext_IASetInputLayout(d3d->DeviceContext, SampleTriangleInfo.InputLayout);
                        ID3D11DeviceContext_IASetPrimitiveTopology(d3d->DeviceContext, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                        UINT stride = sizeof(vertex_data);
                        UINT offset = 0;
                        ID3D11DeviceContext_IASetVertexBuffers(d3d->DeviceContext, 0, 1, &SampleTriangleInfo.VertexBuffer, &stride, &offset);
                        
                        // Vertex Shader
                        ID3D11DeviceContext_VSSetConstantBuffers(d3d->DeviceContext, 0, 1, &SampleTriangleInfo.ConstantBuffer);
                        ID3D11DeviceContext_VSSetShader(d3d->DeviceContext, SampleTriangleInfo.VertexShader, NULL, 0);
                        
                        // Pixel Shader
                        ID3D11DeviceContext_PSSetShader(d3d->DeviceContext, SampleTriangleInfo.PixelShader, NULL, 0);
                        
                        // draw 3 vertices
                        ID3D11DeviceContext_Draw(d3d->DeviceContext, 3, 0);
                        
                        
                    }break;
#endif
                    case RenderInfoType_Square:
                    {
                        render_info SquareInfo = GameState->RenderInfos[RenderInfoType_Square];
                        
                        // Input Assembler
                        UINT stride = sizeof(v3);
                        UINT offset = 0;
                        ID3D11DeviceContext_IASetInputLayout(d3d->DeviceContext, SquareInfo.InputLayout);
                        ID3D11DeviceContext_IASetPrimitiveTopology(d3d->DeviceContext, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                        ID3D11DeviceContext_IASetVertexBuffers(d3d->DeviceContext, 0, 1, &SquareInfo.VertexBuffer, &stride, &offset);
                        ID3D11DeviceContext_IASetIndexBuffer(d3d->DeviceContext, SquareInfo.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
                        
                        // Vertex Shader
                        ID3D11DeviceContext_VSSetConstantBuffers(d3d->DeviceContext, 0, 1, &SquareInfo.ConstantBuffer);
                        ID3D11DeviceContext_VSSetShader(d3d->DeviceContext, SquareInfo.VertexShader, NULL, 0);
                        
                        // Rasterizer Stage
                        ID3D11DeviceContext_RSSetViewports(d3d->DeviceContext, 1, &WindowViewport);
                        ID3D11DeviceContext_RSSetState(d3d->DeviceContext, d3d->RasterizerState);
                        
                        // Pixel Shader
                        ID3D11DeviceContext_PSSetShader(d3d->DeviceContext, SquareInfo.PixelShader, NULL, 0);
                        
                        
                        {
                            // Update the SQUARE constant buffer
                            
                            // This should go:
                            // Model to World
                            // World to View
                            // View to Projection
                            
                            m4 Model = M4InitD(1.0f);
                            
                            v3 Scale = v3(0.5f, 0.5f, 0.0f);
                            Model = M4ScaleV3(Scale);
                            
                            m4 View = M4TranslateV3(V3Negate(Camera->Position));
                            m4 ModelView = M4MultiplyM4(Model, View);
                            
                            m4 ModelViewProjection = M4MultiplyM4(ModelView, Camera->Perspective);
                            
                            square_constant ConstantData = 
                            {
                                {-0.00f, +0.00f, 0.30f, 0.00f}, // cPos
                                //Cos(angle_square), Sin(angle_square), 0.0f, 0.0f,
                                
                                // TODO(Eric): Size does nothing in the shader atm. How would we do that?
                                {0.0f, 0.0f, 0.0f, 0.0f}, // cSize
                                
                                //0.8f, 0.8f, 0.8f, 1.0f   // cColor
                                {Cos(angle), Sin(angle), 0.8f, 1.0f},
                                
                                ModelViewProjection
                            };
                            
                            D3D11_MAPPED_SUBRESOURCE ConstantSubresource;
                            hr = ID3D11DeviceContext_Map(d3d->DeviceContext, (ID3D11Resource*)SquareInfo.ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantSubresource);
                            AssertHR(hr);
                            memcpy(ConstantSubresource.pData, &ConstantData, sizeof(ConstantData));
                            ID3D11DeviceContext_Unmap(d3d->DeviceContext, (ID3D11Resource*)SquareInfo.ConstantBuffer, 0);
                            
                            // Draw 6 vertices, from the 4 indexes we have set in the vertex buffer (two triangles)
                            ID3D11DeviceContext_DrawIndexed(d3d->DeviceContext, 6, 0, 0);
                        }
                        
                        //~ NOTE(Eric): Test draw a second square, with a different z-value to test depth
                        // Lower Z == closer to the camera
                        {
                            m4 Model = M4InitD(1.0f);
                            
                            v3 Scale = v3(0.5f, 0.5f, 0.0f);
                            Model = M4ScaleV3(Scale);
                            
                            // NOTE(Eric): Testing out an 'offset', to hopefully change it's position.
                            // However, I'm manually setting the w in the shader, so I don't think this is used.
                            
                            //-
                            // Also IMPORTANT: I'm going back to arcsync tutorials, and my notes on them, because they are great.
                            // My new idea is to go through them again, along with my notes, and convert the things to dx11,
                            // so we have a solid understanding of all of it from the ground up.
                            //-
                            
                            /*
                                                                                    v3 Offset = v3(-0.5f, 1.5f, 0.0f);
                                                                                    Model.elements[3][0] = Offset.x;
                                                                                    Model.elements[3][1] = Offset.y;
                                                                                    Model.elements[3][2] = Offset.z;
                                                                                    Model.elements[3][3] = 1.0f;
                                                                                    */
                            
                            // NOTE(Eric): What I should do now is figure out how to use my own coordinates.
                            
                            m4 View = M4TranslateV3(V3Negate(Camera->Position));
                            m4 ModelView = M4MultiplyM4(Model, View);
                            
                            m4 ModelViewProjection = M4MultiplyM4(ModelView, Camera->Perspective);
                            
                            square_constant ConstantData[] = 
                            {
                                +0.20f, +0.40f, 0.40f, 0.00f, // cPos
                                //Cos(angle_square), Sin(angle_square), 0.0f, 0.0f,
                                
                                // TODO(Eric): Size does nothing in the shader atm. How would we do that?
                                0.0f, 0.0f, 0.0f, 0.0f, // cSize
                                
                                0.8f, 0.8f, 0.8f, 1.0f,   // cColor
                                //Cos(angle_square), Sin(angle_square), 0.8f, 1.0f
                                
                                ModelViewProjection
                            };
                            
                            D3D11_MAPPED_SUBRESOURCE mapped;
                            hr = ID3D11DeviceContext_Map(d3d->DeviceContext, (ID3D11Resource*)SquareInfo.ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
                            AssertHR(hr);
                            memcpy(mapped.pData, ConstantData, sizeof(ConstantData));
                            ID3D11DeviceContext_Unmap(d3d->DeviceContext, (ID3D11Resource*)SquareInfo.ConstantBuffer, 0);
                            
                            ID3D11DeviceContext_DrawIndexed(d3d->DeviceContext, 6, 0, 0);
                            
                        }
                        
                        // NOTE(Eric): Draw many squares?
                        if (0)
                        {
                            v3 Scale = v3(0.2f, 0.2f, 0.0f);
                            m4 Model = M4ScaleV3(Scale);
                            m4 View = M4TranslateV3(V3Negate(Camera->Position));
                            m4 ModelView = M4MultiplyM4(Model, View);
                            m4 ModelViewProjection = M4MultiplyM4(ModelView, Camera->Perspective);
                            
                            for (u32 i = 0; i < 16; i++)
                            {
                                f32 NewX = Lerp(-1.0f, 1.0f, i / 15.0f);
                                
                                for (u32 j = 0; j < 16; j++)
                                {
                                    f32 NewY = Lerp(-1.0f, 1.0f, j / 15.0f);
                                    
                                    square_constant ConstantData[] = 
                                    {
                                        NewX - 1.5f, NewY, 0.10f, 0.00f, // cPos
                                        //Cos(angle_square), Sin(angle_square), 0.0f, 0.0f,
                                        
                                        // TODO(Eric): Size does nothing in the shader atm. How would we do that?
                                        0.0f, 0.0f, 0.0f, 0.0f, // cSize
                                        
                                        NewX, NewY, 0.2f, 1.0f,   // cColor
                                        //Cos(angle_square), Sin(angle_square), 0.8f, 1.0f
                                        
                                        ModelViewProjection
                                    };
                                    
                                    D3D11_MAPPED_SUBRESOURCE mapped;
                                    hr = ID3D11DeviceContext_Map(d3d->DeviceContext, (ID3D11Resource*)SquareInfo.ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
                                    AssertHR(hr);
                                    memcpy(mapped.pData, ConstantData, sizeof(ConstantData));
                                    ID3D11DeviceContext_Unmap(d3d->DeviceContext, (ID3D11Resource*)SquareInfo.ConstantBuffer, 0);
                                    
                                    ID3D11DeviceContext_DrawIndexed(d3d->DeviceContext, 6, 0, 0);
                                }
                            }
                        }
                    }break;
                }
                
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



/*
TODO(Eric) (9/10/2021):

Simplify and Comment!
Remove almost everything uneeded and comment everything that's left.
- like the sampler state, blend state, etc.
Just get a simple triangle up, solid color
Simplify the functions, narrow it down to only one or two

Comment everything, make it extremely clear what each thing is _for_, and what they connect to.
Ex: The constant buffer lined up with this specific variable in the shader,
the vertex buffer data comes in through this, etc. etc.

and THEN I can start expirementing:
- Draw a square
- Draw _many_ squares
- Use my own coordinates
- Get things moving, probably with constant buffers
- etc!


*/

/*
TODO(Eric): (9/20/2021):

Now I'd like to set up a single vertex buffer that defines what a square is.
Then adjust a constant buffer with a position, size, and color in it.
Then when I go to render each square, it updates the constant buffer with it's info and draws it
Each draw square call is the same, uses the same shaders, vertex buffer, index buffer, etc.
The only difference per square is the constant buffer.

but FIRST, let's try to change our current square with a constant buffer.

*/

/*
TODO(Eric): (9/21/2021):

I'm not sure how to effect the size of the square.
It's directly based on what vertices we are giving it,
and since the shaders work on single vertices at a time (but multiple at a time),
I don't know how to just change a couple of them. Right?
Oh, you know what.
It's on the transform, which is going straight into matrix multiplication transforms
those do size, rotation and scale. I did a bunch of exmaples on each of those already.

*/

/*
TODO(Eric): (9/23/2021):

I've gotten the game_state into the permanent_arena, so that's good
Now I'm trying to get the delta time from the win32 timer..
os->GetTime() gets a cumulative time the app has been running

we are getting in pretty good shape, I've done a bunch of things I wanted to do
It should be easier to draw more things, just need to make a few functions to help
It still seems a little messy, but it's not bad. It's still really clear what's happening
there's just some really long lines

    Things to learn immediately:
    - projection matrix + world coordinates, and how I can make it 2d
      -- I think this can just be the regular 3d camera, but the camera movement is limited. only moves side-side, up-down
- depth testing. How to handle z values in a 2d game


*/

/*
TODO(Eric): (9/30/2021):

Triangle is setting Z of 0 in the shader
My Squares are 0 on init, and adding the constant buffer for its Z value

OK. So two rectangles, the z value does effect which is in front
When they are the same, the last one rendered is in front.
The triangle's Z value is 0, but is always in front...

With how it's set up currently, a higher Z value is closer and shows on top.


*/




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