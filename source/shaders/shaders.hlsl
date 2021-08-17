
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
    float4x2 uTransform;
}

// s0 = sampler bound to slot 0
sampler sampler0 : register(s0);

// t0 = shader resource bound to slot 0
Texture2D<float4> texture0 : register(t0);

PS_INPUT vs(VS_INPUT input)
{
    // NOTE(Eric): Commented out because first test is not using the matrix transform
    float2 pos = mul(uTransform, input.pos).xy;
    //float2 pos = input.pos;
    
    PS_INPUT output;
    output.pos = float4(pos, 0.f, 1.f);
    output.uv = input.uv;
    output.color = float4(input.color, 1.f);
    return output;
}

float4 ps(PS_INPUT input) : SV_TARGET
{
    float4 tex = texture0.Sample(sampler0, input.uv);
    return input.color * tex;
}