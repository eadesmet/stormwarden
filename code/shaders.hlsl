
/*
cbuffer constants : register(b0)
{
    float2 offset;
    float4 uniformColor;
};


cbuffer constants_mat : register(b1)
{
    float4x4 modelViewProj;
};
*/

cbuffer constants : register(b0)
{
    float4x4 modelViewProj;
};


struct VS_Input
{
	float2 pos : POS;
	float2 uv  : TEX;
};

struct VS_Output 
{
    float4 pos   : SV_POSITION;
    //float4 color : COLOR;
	float2 uv    : TEXCOORD;
};

Texture2D    mytexture : register(t0);
SamplerState mysampler : register(s0);

VS_Output vs_main(VS_Input input)
{
    VS_Output output;
    // Remember, Adding an offset here from constants gets you to move things
	output.pos = mul(float4(input.pos, 0.0f, 1.0f), modelViewProj);
	output.uv = input.uv;

	//output.color = uniformColor;

    return output;
}

float4 ps_main(VS_Output input) : SV_Target
{
    //return input.color;   
	return mytexture.Sample(mysampler, input.uv);
}
