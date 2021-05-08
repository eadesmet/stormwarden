/* date = May 7th 2021 3:31 pm */

#ifndef STATE_H
#define STATE_H

enum V_SHADER
{
    V_SHADER_GROUND,
    V_SHADER_PLAYER,
    
    V_SHADER_COUNT
};
enum P_SHADER
{
    P_SHADER_BASIC,
    
    P_SHADER_COUNT
};
struct d3d_shaders
{
    ID3D11VertexShader* VertexShader[V_SHADER_COUNT];
    ID3D11InputLayout* InputLayout[V_SHADER_COUNT];
    
    ID3D11PixelShader* PixelShader[P_SHADER_COUNT];
};


struct d3d_state
{
    ID3D11Device1* Device;
    ID3D11DeviceContext1* DeviceContext;
    
    IDXGISwapChain1* SwapChain;
    
    d3d_shaders Shaders;
};

#endif //STATE_H
