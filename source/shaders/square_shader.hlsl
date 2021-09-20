
struct VS_INPUT
{
    // these names must match D3D11_INPUT_ELEMENT_DESC array
    float2 pos   : POSITION;
    float2 uv    : TEXCOORD;
    float3 color : COLOR;
};

struct PS_INPUT
{
    // these names do not matter, except SV_... ones
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD;
    float4 color : COLOR;
};


// b0 = constant buffer bound to slot 0
cbuffer cbuffer0 : register(b0)
{
    float3 cPos;
    float3 cSize;
    float4 cColor;
}


PS_INPUT vs(VS_INPUT input)
{
    float2 pos = input.pos;
    
    PS_INPUT output;
    output.pos = float4(pos, 0.f, 1.f) + float4(cPos, 0.f);
    output.uv = input.uv;
    output.color = cColor;//float4(input.color, 1.f);
    return output;
}

float4 ps(PS_INPUT input) : SV_TARGET
{
    return input.color;
}