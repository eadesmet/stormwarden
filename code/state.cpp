
#include "state.h"

// TODO(Eric): I'm not sure if any of this makes sense to do
// TODO(Eric): Don't be afraid of deleting it.

// TODO(Eric): One problem with it already is the shared input layouts.
// If one input layout matches multiple shaders, wouldn't we want to reuse it?



internal void
CreateInputLayout(d3d_state *DState, ID3DBlob* VSBlob, V_SHADER Type)
{
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
    HRESULT hResult = DState->Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &DState->Shaders.InputLayout[Type]);
    assert(SUCCEEDED(hResult));
    
    VSBlob->Release();
}

internal ID3DBlob*
CreateVertexShader(d3d_state *DState, V_SHADER Type, LPCWSTR ShaderFilePath)
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
    
    return vsBlob;
}

internal void
InitVertexShaderAndInputLayout(d3d_state *DState, V_SHADER Type, LPCWSTR ShaderFilePath)
{
    ID3DBlob* vsBlob = CreateVertexShader(DState, Type, ShaderFilePath);
    CreateInputLayout(DState, vsBlob, Type);
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

