

struct VS_Input
{
	float2 pos : POS;
};

struct VS_Output 
{
    float4 pos   : SV_POSITION;
};


VS_Output vs_main(VS_Input input)
{
	VS_Output Result;
	Result.pos = float4(input.pos, 0.0f, 1.0f);
	return Result;
}

float4 ps_main(VS_Output input) : SV_Target
{
	return float4(0.6f, 0.2f, 0.2f, 1.0f);
}