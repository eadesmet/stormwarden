
#include "state.h"

// TODO(Eric): Don't be afraid of deleting all this.


internal void
InitVertexShaderAndInputLayout(d3d_state *DState, V_SHADER Type, LPCWSTR ShaderFilePath)
{
    // Create Vertex Shader
    ID3DBlob* vsBlob;
    ID3DBlob* shaderCompileErrorsBlob;
    
    HRESULT hResult = D3DCompileFromFile(ShaderFilePath, nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vsBlob, &shaderCompileErrorsBlob);
    if(FAILED(hResult))
    {
        const char* errorString = NULL;
        if(hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            errorString = "Could not compile shader; file not found";
        else if(shaderCompileErrorsBlob){
            errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
            shaderCompileErrorsBlob->Release();
        }
        MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
        assert(0);
    }
    
    hResult = DState->Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &DState->Shaders.VertexShader[Type]);
    assert(SUCCEEDED(hResult));
    
    // TODO(Eric): One problem with it already is the shared input layouts.
    // If one input layout matches multiple shaders, wouldn't we want to reuse it?
    
    // Create the Input Layout
    switch(Type)
    {
        case V_SHADER_STATIC:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
            {
                { "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
            hResult = DState->Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &DState->Shaders.InputLayout[Type]);
            assert(SUCCEEDED(hResult));
        }break;
        case V_SHADER_TEXTURE:
        {
            D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
            {
                { "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                
                { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };
            hResult = DState->Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &DState->Shaders.InputLayout[Type]);
            assert(SUCCEEDED(hResult));
        } break;
        default:
        {
            assert(0);
        }break;
    }
    
    vsBlob->Release();
}


//~ NOTE(Eric): Pixel Shaders

internal void
InitPixelShader(d3d_state *DState, P_SHADER Type, LPCWSTR ShaderFilePath)
{
    // Create Pixel Shader
    ID3DBlob* psBlob;
    ID3DBlob* shaderCompileErrorsBlob;
    
    HRESULT hResult = D3DCompileFromFile(ShaderFilePath, nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &shaderCompileErrorsBlob);
    if(FAILED(hResult))
    {
        const char* errorString = NULL;
        if(hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            errorString = "Could not compile shader; file not found";
        else if(shaderCompileErrorsBlob){
            errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
            shaderCompileErrorsBlob->Release();
        }
        MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
        assert(0);
    }
    
    hResult = DState->Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &DState->Shaders.PixelShader[Type]);
    assert(SUCCEEDED(hResult));
    psBlob->Release();
    
    
}

