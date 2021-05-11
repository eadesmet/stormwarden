#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define UNICODE
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

#define internal static

static bool g_WindowDidResize = false;

static LPCWSTR g_ShaderFileFullPath = L"../code/shaders.hlsl";
static LPCWSTR g_v_shader_static_filepath = L"../code/vs_static.hlsl";


#include "math.h"
#include "state.cpp"

// Input
enum GameAction {
    GameAction_W,
    GameAction_A,
    GameAction_S,
    GameAction_D,
    GameAction_LeftArrow,
    GameAction_RightArrow,
    GameAction_UpArrow,
    GameAction_DownArrow,
    GameAction_Q,
    GameAction_E,
    GameActionCount
};
static bool g_keyIsDown[GameActionCount] = {};


LRESULT CALLBACK 
WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;
    switch(message)
    {
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            bool isDown = (message == WM_KEYDOWN);
            switch(wParam)
            {
                case VK_ESCAPE:
                {
                    DestroyWindow(hWnd);
                    break;
                }
                case 'W':
                {
                    g_keyIsDown[GameAction_W] = isDown;
                    break;
                }
                case 'A':
                {
                    g_keyIsDown[GameAction_A] = isDown;
                    break;
                }
                case 'S':
                {
                    g_keyIsDown[GameAction_S] = isDown;
                    break;
                }
                case 'D':
                {
                    g_keyIsDown[GameAction_D] = isDown;
                    break;
                }
                case 'Q':
                {
                    g_keyIsDown[GameAction_Q] = isDown;
                    break;
                }
                case 'E':
                {
                    g_keyIsDown[GameAction_E] = isDown;
                    break;
                }
                case VK_UP:
                {
                    g_keyIsDown[GameAction_UpArrow] = isDown;
                    break;
                }
                case VK_DOWN:
                {
                    g_keyIsDown[GameAction_DownArrow] = isDown;
                    break;
                }
                case VK_LEFT:
                {
                    g_keyIsDown[GameAction_LeftArrow] = isDown;
                    break;
                }
                case VK_RIGHT:
                {
                    g_keyIsDown[GameAction_RightArrow] = isDown;
                    break;
                }
            }
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
        {
            g_WindowDidResize = true;
            break;
        }
        default:
        {
            Result = DefWindowProcW(hWnd, message, wParam, lParam);
            break;
        }
    }
    
    return Result;
}

int WINAPI 
WinMain(HINSTANCE hInstance,
        HINSTANCE /*hPrevInstance*/,
        LPSTR     /*lpCmdLine*/,
        int       /*nCmdShow*/)
{
    // Open a window
    HWND hwnd;
    {
        WNDCLASSEXW winClass = {};
        winClass.cbSize = sizeof(WNDCLASSEXW);
        winClass.style = CS_HREDRAW | CS_VREDRAW;
        winClass.lpfnWndProc = &WindowProc;
        winClass.hInstance = hInstance;
        winClass.hIcon = LoadIconW(0, IDI_APPLICATION);
        winClass.hCursor = LoadCursorW(0, IDC_ARROW);
        winClass.lpszClassName = L"MyWindowClass";
        winClass.hIconSm = LoadIconW(0, IDI_APPLICATION);
        
        if(!RegisterClassExW(&winClass)) {
            MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);
            return GetLastError();
        }
        
        RECT initialRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
        LONG initialWidth = initialRect.right - initialRect.left;
        LONG initialHeight = initialRect.bottom - initialRect.top;
        
        hwnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
                               winClass.lpszClassName,
                               L"Stormwarden",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               initialWidth, 
                               initialHeight,
                               0, 0, hInstance, 0);
        
        if(!hwnd) {
            MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);
            return GetLastError();
        }
    }
    
    d3d_state DState = {};
    InitD3D11(&DState, hwnd);
    CreateD3D11RenderTargets(&DState);
    
    // Create Vertex Buffer
    ID3D11Buffer* vertexBuffer;
    UINT numVerts;
    UINT stride;
    UINT offset;
    {
        float vertexData[] = { // x, y, u, v
            -0.5f,  0.5f, 0.0f, 0.0f,  // Upper-left
            0.5f, -0.5f,  1.0f, 1.0f,  // Bottom-Right
            -0.5f, -0.5f, 0.0f, 1.0f,  // Bottom-Left
            
            -0.5f,  0.5f, 0.0f, 0.0f,  // Upper-left (Again! (Triangles!))
            0.5f,  0.5f,  1.0f, 0.0f,   // Upper-Right
            0.5f, -0.5f,  1.0f, 1.0f   // Bottom-Right
        };
        stride = 4 * sizeof(float);
        numVerts = sizeof(vertexData) / stride;
        offset = 0;
        
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.ByteWidth = sizeof(vertexData);
        vertexBufferDesc.Usage     = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertexData };
        
        HRESULT hResult = DState.Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);
        assert(SUCCEEDED(hResult));
    }
    
    // Create Vertex Buffer
    ID3D11Buffer* StaticVertexBuffer;
    ID3D11Buffer* StaticIndexBuffer;
    //UINT Static_numVerts;
    UINT Static_NumIndices;
    UINT Static_stride;
    UINT Static_offset;
    {
        float vertexData[] = 
        { // x, y
            /*
            -0.5f, -0.5f, // 0
            
            -0.5f,  0.5f, // 1
            0.5f, -0.5f,  // 2
            
            0.5f, 0.5f,  // 3
*/
            
            20.0f, 20.0f,
            20.0f, 50.0f,
            50.0f, 20.0f,
            50.0f, 50.0f
                
        };
        
        uint16_t indicies[] = 
        {
            0, 1, 3,
            0, 2, 3,
        };
        Static_stride = 2 * sizeof(float);
        Static_offset = 0;
        Static_NumIndices = sizeof(indicies) / sizeof(indicies[0]);
        
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.ByteWidth = sizeof(vertexData);
        vertexBufferDesc.Usage     = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertexData };
        
        HRESULT hResult = DState.Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &StaticVertexBuffer);
        assert(SUCCEEDED(hResult));
        
        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.ByteWidth = sizeof(indicies);
        indexBufferDesc.Usage     = D3D11_USAGE_IMMUTABLE;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA indexSubresourceData = { indicies };
        
        hResult = DState.Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &StaticIndexBuffer);
        assert(SUCCEEDED(hResult));
    }
    
    // Create Sampler State
    ID3D11SamplerState* samplerState;
    {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_BORDER;
        samplerDesc.BorderColor[0] = 1.0f;
        samplerDesc.BorderColor[1] = 1.0f;
        samplerDesc.BorderColor[2] = 1.0f;
        samplerDesc.BorderColor[3] = 1.0f;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        
        DState.Device->CreateSamplerState(&samplerDesc, &samplerState);
    }
    
    // Load Image
    int texWidth, texHeight, texNumChannels;
    int texForceNumChannels = 4;
    unsigned char* testTextureBytes = stbi_load("../assets/test.png", &texWidth, &texHeight,
                                                &texNumChannels, texForceNumChannels);
    assert(testTextureBytes);
    int texBytesPerRow = 4 * texWidth;
    
    // Create Texture
    ID3D11ShaderResourceView* textureView;
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width              = texWidth;
        textureDesc.Height             = texHeight;
        textureDesc.MipLevels          = 1;
        textureDesc.ArraySize          = 1;
        textureDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        textureDesc.SampleDesc.Count   = 1;
        textureDesc.Usage              = D3D11_USAGE_IMMUTABLE;
        textureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
        
        D3D11_SUBRESOURCE_DATA textureSubresourceData = {};
        textureSubresourceData.pSysMem = testTextureBytes;
        textureSubresourceData.SysMemPitch = texBytesPerRow;
        
        ID3D11Texture2D* texture;
        DState.Device->CreateTexture2D(&textureDesc, &textureSubresourceData, &texture);
        
        DState.Device->CreateShaderResourceView(texture, nullptr, &textureView);
        texture->Release();
    }
    
    free(testTextureBytes);
    
    // NOTE(Eric): On Constant Buffers Flags!
    // "Resist" using D3C11_USAGE_DYNAMIC and D3D11_CPU_ACCESS_WRITE!
    // (The sample code I've been using uses this)
    // We can instead use Usage=DEFAULT and CPUAccess=0,
    // then later use the function DeviceContext->UpdateSubresource
    // UpdateSubresource requires these flags
    
    
    struct Constants
    {
        float4x4 modelViewProj;
    };
    
    ID3D11Buffer* constantBuffer;
    {
        D3D11_BUFFER_DESC constantBufferDesc = {};
        // ByteWidth must be a multiple of 16, per the docs
        constantBufferDesc.ByteWidth      = sizeof(Constants) + 0xf & 0xfffffff0;
        constantBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
        constantBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        HRESULT hResult = DState.Device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
        assert(SUCCEEDED(hResult));
    }
    
    struct Constants_Offset
    {
        float2 offset;
        float2 scale;
        float4 color;
    };
    ID3D11Buffer* constantBuffer_offset;
    {
        D3D11_BUFFER_DESC constantBufferDesc = {};
        // ByteWidth must be a multiple of 16, per the docs
        constantBufferDesc.ByteWidth      = sizeof(Constants_Offset) + 0xf & 0xfffffff0;
        constantBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
        constantBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        HRESULT hResult = DState.Device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer_offset);
        assert(SUCCEEDED(hResult));
    }
    
    ID3D11Buffer* Ortho_CBuffer;
    {
        D3D11_BUFFER_DESC constantBufferDesc = {};
        // ByteWidth must be a multiple of 16, per the docs
        constantBufferDesc.ByteWidth      = sizeof(Constants) + 0xf & 0xfffffff0;
        constantBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
        constantBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        HRESULT hResult = DState.Device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer_offset);
        assert(SUCCEEDED(hResult));
    }
    
    ID3D11Buffer* ConstantBuffers[] = {constantBuffer, constantBuffer_offset, Ortho_CBuffer};
    
    
    ID3D11RasterizerState* rasterizerState;
    {
        D3D11_RASTERIZER_DESC rasterizerDesc = {};
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_NONE; // Show the texture front and back
        rasterizerDesc.FrontCounterClockwise = FALSE; // Which side to show, if culling
        
        DState.Device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    }
    
    ID3D11DepthStencilState* depthStencilState;
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
        depthStencilDesc.DepthEnable    = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;
        
        DState.Device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    }
    
    
    // Camera
    float3 cameraPos = {0, 0, 2};
    float3 cameraFwd = {0, 0, -1};
    float cameraPitch = 0.f;
    float cameraYaw = 0.f;
    
    float4x4 perspectiveMat = {};
    g_WindowDidResize = true; // To force initial perspectiveMat calculation
    
    
    // Timing
    LONGLONG startPerfCount = 0;
    LONGLONG perfCounterFrequency = 0;
    {
        LARGE_INTEGER perfCount;
        QueryPerformanceCounter(&perfCount);
        startPerfCount = perfCount.QuadPart;
        LARGE_INTEGER perfFreq;
        QueryPerformanceFrequency(&perfFreq);
        perfCounterFrequency = perfFreq.QuadPart;
    }
    double currentTimeInSeconds = 0.0;
    
    
    // TODO(Eric): Change these to one function that loads everything.
    // There really isn't a need to have these separate, I would always want to load all shaders
    InitVertexShaderAndInputLayout(&DState, V_SHADER_TEXTURE, g_ShaderFileFullPath);
    InitPixelShader(&DState, P_SHADER_TEXTURE, g_ShaderFileFullPath);
    
    InitVertexShaderAndInputLayout(&DState, V_SHADER_STATIC, g_v_shader_static_filepath);
    InitPixelShader(&DState, P_SHADER_STATIC, g_v_shader_static_filepath);
    
    // Main Loop
    bool isRunning = true;
    while(isRunning)
    {
        float dt;
        {
            double previousTimeInSeconds = currentTimeInSeconds;
            LARGE_INTEGER perfCount;
            QueryPerformanceCounter(&perfCount);
            
            currentTimeInSeconds = (double)(perfCount.QuadPart - startPerfCount) / (double)perfCounterFrequency;
            dt = (float)(currentTimeInSeconds - previousTimeInSeconds);
            if(dt > (1.f / 60.f))
                dt = (1.f / 60.f);
        }
        
        
        MSG msg = {};
        while(PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
                isRunning = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        
        // Get window dimensions
        int windowWidth, windowHeight;
        float windowAspectRatio;
        {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            windowWidth = clientRect.right - clientRect.left;
            windowHeight = clientRect.bottom - clientRect.top;
            windowAspectRatio = (float)windowWidth / (float)windowHeight;
        }
        
        if(g_WindowDidResize)
        {
            DState.DeviceContext->OMSetRenderTargets(0, 0, 0);
            DState.FrameBufferView->Release();
            DState.DepthBufferView->Release();
            
            HRESULT res = DState.SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            assert(SUCCEEDED(res));
            
            CreateD3D11RenderTargets(&DState);
            
            perspectiveMat = makePerspectiveMat(windowAspectRatio, degreesToRadians(84), 0.1f, 1000.f);
            
            g_WindowDidResize = false;
        }
        
        // NOTE(Eric): NEW CAMERA STUFF
        // Update camera
        {
            float3 camFwdXZ = normalise({cameraFwd.x, 0, cameraFwd.z});
            float3 cameraRightXZ = cross(camFwdXZ, {0, 1, 0});
            
            const float CAM_MOVE_SPEED = 5.f; // in metres per second
            const float CAM_MOVE_AMOUNT = CAM_MOVE_SPEED * dt;
            if(g_keyIsDown[GameAction_W])
                cameraPos += camFwdXZ * CAM_MOVE_AMOUNT;
            if(g_keyIsDown[GameAction_S])
                cameraPos -= camFwdXZ * CAM_MOVE_AMOUNT;
            if(g_keyIsDown[GameAction_A])
                cameraPos -= cameraRightXZ * CAM_MOVE_AMOUNT;
            if(g_keyIsDown[GameAction_D])
                cameraPos += cameraRightXZ * CAM_MOVE_AMOUNT;
            if(g_keyIsDown[GameAction_Q])
                cameraPos.y += CAM_MOVE_AMOUNT;
            if(g_keyIsDown[GameAction_E])
                cameraPos.y -= CAM_MOVE_AMOUNT;
            
            const float CAM_TURN_SPEED = M_PI; // in radians per second
            const float CAM_TURN_AMOUNT = CAM_TURN_SPEED * dt;
            if(g_keyIsDown[GameAction_LeftArrow])
                cameraYaw += CAM_TURN_AMOUNT;
            if(g_keyIsDown[GameAction_RightArrow])
                cameraYaw -= CAM_TURN_AMOUNT;
            if(g_keyIsDown[GameAction_UpArrow])
                cameraPitch += CAM_TURN_AMOUNT;
            if(g_keyIsDown[GameAction_DownArrow])
                cameraPitch -= CAM_TURN_AMOUNT;
            
            // Wrap yaw to avoid floating-point errors if we turn too far
            while(cameraYaw >= 2*M_PI) 
                cameraYaw -= 2*M_PI;
            while(cameraYaw <= -2*M_PI) 
                cameraYaw += 2*M_PI;
            
            // Clamp pitch to stop camera flipping upside down
            if(cameraPitch > degreesToRadians(85)) 
                cameraPitch = degreesToRadians(85);
            if(cameraPitch < -degreesToRadians(85)) 
                cameraPitch = -degreesToRadians(85);
        }
        
        // Calculate view matrix from camera data
        // 
        // float4x4 viewMat = inverse(rotateXMat(cameraPitch) * rotateYMat(cameraYaw) * translationMat(cameraPos));
        // NOTE: We can simplify this calculation to avoid inverse()!
        // Applying the rule inverse(A*B) = inverse(B) * inverse(A) gives:
        // float4x4 viewMat = inverse(translationMat(cameraPos)) * inverse(rotateYMat(cameraYaw)) * inverse(rotateXMat(cameraPitch));
        // The inverse of a rotation/translation is a negated rotation/translation:
        float4x4 viewMat = translationMat(-cameraPos) * rotateYMat(-cameraYaw) * rotateXMat(-cameraPitch);
        // Update the forward vector we use for camera movement:
        cameraFwd = {-viewMat.m[2][0], -viewMat.m[2][1], -viewMat.m[2][2]};
        
        // Spin the quad
        float4x4 modelMat = rotateYMat(0.2f * (float)(M_PI * currentTimeInSeconds));
        
        // Calculate model-view-projection matrix to send to shader
        float4x4 modelViewProj = modelMat * viewMat * perspectiveMat;
        
        // Update constant buffer
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        DState.DeviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
        Constants* constants = (Constants*)(mappedSubresource.pData);
        constants->modelViewProj = modelViewProj;
        DState.DeviceContext->Unmap(constantBuffer, 0);
        
        
        FLOAT backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
        DState.DeviceContext->ClearRenderTargetView(DState.FrameBufferView, backgroundColor);
        
        
        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)windowWidth, (FLOAT)windowHeight, 0.0f, 1.0f };
        
        // Rasterizer Stage
        DState.DeviceContext->RSSetViewports(1, &viewport);
        DState.DeviceContext->RSSetState(rasterizerState);
        
        // Input Assembler Stage
        DState.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        DState.DeviceContext->IASetInputLayout(DState.Shaders.InputLayout[V_SHADER_TEXTURE]);
        DState.DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        
        // Vertex Shader Stage
        DState.DeviceContext->VSSetShader(DState.Shaders.VertexShader[V_SHADER_TEXTURE], nullptr, 0);
        //DState.DeviceContext->VSSetConstantBuffers(0, 2, ConstantBuffers);
        DState.DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffers[0]);
        
        // Pixel Shader Stage
        DState.DeviceContext->PSSetShader(DState.Shaders.PixelShader[P_SHADER_TEXTURE], nullptr, 0);
        DState.DeviceContext->PSSetShaderResources(0, 1, &textureView);
        DState.DeviceContext->PSSetSamplers(0, 1, &samplerState);
        
        // Output Merger Stage
        DState.DeviceContext->OMSetRenderTargets(1, &DState.FrameBufferView, nullptr);
        DState.DeviceContext->OMSetDepthStencilState(depthStencilState, 0);
        
        DState.DeviceContext->Draw(numVerts, 0);
        
        
        
        
        
        
        DState.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        DState.DeviceContext->IASetInputLayout(DState.Shaders.InputLayout[V_SHADER_STATIC]);
        DState.DeviceContext->IASetVertexBuffers(0, 1, &StaticVertexBuffer, &Static_stride, &Static_offset);
        DState.DeviceContext->IASetIndexBuffer(StaticIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        
        DState.DeviceContext->VSSetShader(DState.Shaders.VertexShader[V_SHADER_STATIC], nullptr, 0);
        DState.DeviceContext->PSSetShader(DState.Shaders.PixelShader[P_SHADER_STATIC], nullptr, 0);
        
        
        //DState.DeviceContext->Draw(Static_numVerts, 0);
        //DState.DeviceContext->DrawIndexed(Static_NumIndices, 0, 0);
        
        
        // Clear the Offsets so they don't get continuously incremented:
        // Update constant buffer
        D3D11_MAPPED_SUBRESOURCE Offset_mappedSubresource;
        DState.DeviceContext->Map(ConstantBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &Offset_mappedSubresource);
        Constants_Offset* o_constants = (Constants_Offset*)(Offset_mappedSubresource.pData);
        o_constants->offset = {0.0f, 0.0f};
        o_constants->scale = {0.0f,0.0f};
        o_constants->color = {0.0f, 1.0f, 0.0f, 1.0f};
        DState.DeviceContext->Unmap(ConstantBuffers[1], 0);
        
        // Set the Orthographic Matrix constant (trying this instead of the scale thing)
        D3D11_MAPPED_SUBRESOURCE Ortho_mappedSubresource;
        DState.DeviceContext->Map(ConstantBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &Ortho_mappedSubresource);
        Constants* Ortho_constants = (Constants*)(Ortho_mappedSubresource.pData);
        Ortho_constants->modelViewProj = ;
        
        DState.DeviceContext->Unmap(ConstantBuffers[2], 0);
        
        
        
        DState.DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffers[1]);
        for (int i=0;i<10;i++)
        {
            for (int j=0;j<10;j++)
            {
                
                // Update constant buffer
                Offset_mappedSubresource;
                DState.DeviceContext->Map(ConstantBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &Offset_mappedSubresource);
                o_constants = (Constants_Offset*)(Offset_mappedSubresource.pData);
                o_constants->offset = {(float)i*1.1f,(float)j*1.1f};
                o_constants->scale = {50.0f,0.1f};//{1.0f,1.0f};//{0.2f,0.2f};
                o_constants->color = {0.9f, (float)i/10, (float)j/10, 0.2f};//{0.9f,0.2f,0.2f,0.2f};
                DState.DeviceContext->Unmap(ConstantBuffers[1], 0);
                
                
                
                
                DState.DeviceContext->DrawIndexed(Static_NumIndices, 0, 0);
            }
        }
        
        
        
        
        
        DState.SwapChain->Present(1, 0);
    }
    
    return 0;
}

















#if 0
// Update constant buffer
D3D11_MAPPED_SUBRESOURCE mappedSubresourceMat;
DState.DeviceContext->Map(ConstantBufferMat, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresourceMat);
Constants_Mat* constants_mat = (Constants_Mat*)(mappedSubresourceMat.pData);
constants_mat->modelViewProj = modelViewProj;
DState.DeviceContext->Unmap(ConstantBufferMat, 0);
// NOTE(Eric): END NEW CAMERA STUFF


// Modulate player's y-position
float2 playerPos = {};
const float posCycleAmplitude = 0.5f;
const float posCyclePeriod = 3.f; // in seconds
const float posCycleFreq = 2 * M_PI / posCyclePeriod;
playerPos.y = posCycleAmplitude * sinf(posCycleFreq * (float)currentTimeInSeconds);

// Cycle player color
float4 playerColor;
const float colorCyclePeriod = 5.f; //in seconds
const float colorCycleFreq = 2 * M_PI / colorCyclePeriod;
playerColor.x = 0.5f * (sinf(colorCycleFreq * (float)currentTimeInSeconds) + 1);
playerColor.y = 1 - playerColor.x;
playerColor.z = 0.f;
playerColor.w = 1.f;

// Update constant buffer
D3D11_MAPPED_SUBRESOURCE mappedSubresource;
DState.DeviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
Constants* constants = (Constants*)(mappedSubresource.pData);
constants->pos = playerPos;
constants->color = playerColor;
DState.DeviceContext->Unmap(constantBuffer, 0);
#endif