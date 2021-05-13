
cbuffer constants : register(b0)
{
    float2 offset;
	float2 scale;
    float4 uniformColor;
};

cbuffer constants : register(b1)
{
    float4x4 OrthographicMatrix;
};


struct VS_Input
{
	float2 pos : POS;
};

struct VS_Output 
{
    float4 pos   : SV_POSITION;
	float4 color : COLOR;
};


VS_Output vs_main(VS_Input input)
{
	VS_Output Result;
	float2 newpos = input.pos + offset;
	//Result.pos = float4(mul(newpos, scale.x), 0.0f, 1.0f);
	
	//newpos.x = ((newpos.x / scale.x) * 2) - 1;
	//newpos.y = ((newpos.y / scale.y) * 2) - 1;
	//Result.pos = float4(newpos, 0.0f, 1.0f);

	Result.pos = float4(((newpos / scale.x) * 2) - 1, 0.0f, 1.0f);
	//Result.pos = mul(float4(newpos, 0.0f, 1.0f), OrthographicMatrix);

	//Result.pos = float4(input.pos, 0.0f, 1.0f);
	Result.color = uniformColor;
	return Result;
}

float4 ps_main(VS_Output input) : SV_Target
{
	// Do I have access to constants in pixel shader?
	//return float4(0.6f, 0.2f, 0.2f, 1.0f);
	return input.color;
}