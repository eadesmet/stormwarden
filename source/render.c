
#include "render.h"


internal void
InitRenderer(d3d11_info *d3d, game_state *GameState)
{
    HRESULT hr;
    
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
        hr = ID3D11Device_CreateBuffer(d3d->Device, &desc, &initial, &GameState->RenderInfos[RenderInfoType_SampleTriangle].VertexBuffer);
        AssertHR(hr);
    }
    
    // NOTE(Eric): Attempting to draw a Square.
    {
        /*
        vertex_data my_data[] =
        {
            { { -0.75f, +0.50f }, { 25.0f, 50.0f }, { 1, 0, 0 } }, // Top-left
            { { +0.75f, -0.50f }, {  0.0f,  0.0f }, { 0, 1, 0 } }, // Bottom-right
            { { -0.75f, -0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } }, // Bottom-left
            { { +0.75f, +0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } }, // Top-right
        };
        */
        v3 SquareVertices[] =
        {
            //{ -0.50f, +0.50f, 0.00f }, // Top-left
            //{ +0.50f, -0.50f, 0.00f }, // Bottom-right
            //{ -0.50f, -0.50f, 0.00f }, // Bottom-left
            //{ +0.50f, +0.50f, 0.00f }  // Top-right
            
            { -1.00f, +1.00f, 0.00f }, // Top-left
            { +1.00f, -1.00f, 0.00f }, // Bottom-right
            { -1.00f, -1.00f, 0.00f }, // Bottom-left
            { +1.00f, +1.00f, 0.00f }  // Top-right
        };
        
        D3D11_BUFFER_DESC desc =
        {
            .ByteWidth = sizeof(SquareVertices),
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };
        
        D3D11_SUBRESOURCE_DATA initial = { .pSysMem = SquareVertices };
        hr = ID3D11Device_CreateBuffer(d3d->Device, &desc, &initial, &GameState->RenderInfos[RenderInfoType_Square].VertexBuffer);
        AssertHR(hr);
        
        // Set up an Index Buffer for the square
        u32 indices[] = { 0, 1, 2, 0, 1, 3 };
        
        D3D11_BUFFER_DESC IndexDesc = 
        {
            .ByteWidth = sizeof(SquareVertices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
        };
        D3D11_SUBRESOURCE_DATA IndexDataBuffer = { .pSysMem = indices };
        hr = ID3D11Device_CreateBuffer(d3d->Device, &IndexDesc, &IndexDataBuffer, &GameState->RenderInfos[RenderInfoType_Square].IndexBuffer);
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
        hr = ID3D11Device_CreateVertexShader(d3d->Device, d3d11_vshader, sizeof(d3d11_vshader), NULL, &GameState->RenderInfos[RenderInfoType_SampleTriangle].VertexShader);
        AssertHR(hr);
        
#include "shaders/d3d11_pshader.h"
        hr = ID3D11Device_CreatePixelShader(d3d->Device, d3d11_pshader, sizeof(d3d11_pshader), NULL, &GameState->RenderInfos[RenderInfoType_SampleTriangle].PixelShader);
        AssertHR(hr);
        
        hr = ID3D11Device_CreateInputLayout(d3d->Device, desc, _countof(desc), d3d11_vshader, sizeof(d3d11_vshader), &GameState->RenderInfos[RenderInfoType_SampleTriangle].InputLayout);
        AssertHR(hr);
        
        
        // NOTE(Eric): Square shader
        D3D11_INPUT_ELEMENT_DESC square_desc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        
#include "shaders/d3d11_square_vshader.h"
        hr = ID3D11Device_CreateVertexShader(d3d->Device, d3d11_square_vshader, sizeof(d3d11_square_vshader), NULL, &GameState->RenderInfos[RenderInfoType_Square].VertexShader);
        AssertHR(hr);
        
#include "shaders/d3d11_square_pshader.h"
        hr = ID3D11Device_CreatePixelShader(d3d->Device, d3d11_square_pshader, sizeof(d3d11_square_pshader), NULL, &GameState->RenderInfos[RenderInfoType_Square].PixelShader);
        AssertHR(hr);
        
        hr = ID3D11Device_CreateInputLayout(d3d->Device, square_desc, _countof(square_desc), d3d11_square_vshader, sizeof(d3d11_square_vshader), &GameState->RenderInfos[RenderInfoType_Square].InputLayout);
        AssertHR(hr);
        
    }
    
    
    // Create Constant Buffer, used only for the Transform matrix multiply
    {
        D3D11_BUFFER_DESC SampleTriangleBufferDesc =
        {
            .ByteWidth = 2 * 4 * sizeof(float),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };
        hr = ID3D11Device_CreateBuffer(d3d->Device, &SampleTriangleBufferDesc, NULL, &GameState->RenderInfos[RenderInfoType_SampleTriangle].ConstantBuffer);
        AssertHR(hr);
    }
    
    // Constant buffer for the SQUARE
    {
        D3D11_BUFFER_DESC SquareBufferDesc =
        {
            // {{v4(POS)}, {v4(SIZE)}, {v4(COLOR_}}
            // NOTE(Eric): ByteWidth MUST be a multiple of 16
            .ByteWidth = ((4 * 3) + ((4 * 4) * 3)) * sizeof(float),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };
        hr = ID3D11Device_CreateBuffer(d3d->Device, &SquareBufferDesc, NULL, &GameState->RenderInfos[RenderInfoType_Square].ConstantBuffer);
        AssertHR(hr);
    }
    
}