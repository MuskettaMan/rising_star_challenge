cbuffer cbChangedPerFrame : register(b0)
{
	matrix mvp;
};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float4 color : COLOR;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 color : COLOR;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, mvp);
	vsOut.tex0 = vertex.tex0;
	vsOut.color = vertex.color;
	return vsOut;
}

float4 PS_Main(PS_Input frag) : SV_TARGET
{
	return frag.color;
}
