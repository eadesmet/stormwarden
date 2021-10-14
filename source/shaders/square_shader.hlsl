
struct VS_INPUT
{
    // these names must match D3D11_INPUT_ELEMENT_DESC array
    float3 pos   : POSITION;
    //float2 uv    : TEXCOORD;
    //float3 color : COLOR;
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
    float4 cPos;
    float4 cSize;
    float4 cColor;
    
    float4x4 cModelViewProj;
}


PS_INPUT vs(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 InputPos = float4(input.pos, 1.f) + cPos;
    
    float4 MultipliedPos = mul(InputPos, cModelViewProj);
    //output.pos = InputPos;
    
    //output.pos = float4(MultipliedPos.xy, cPos.z, 1.f);
    output.pos = float4(MultipliedPos.xyz, 1.f);
    
    //output.pos = MultipliedPos;
    
    output.uv = float2(0.f, 0.f); //input.uv;
    output.color = cColor;//float4(input.color, 1.f);
    return output;
}

float4 ps(PS_INPUT input) : SV_TARGET
{
    return input.color;
}