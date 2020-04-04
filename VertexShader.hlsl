cbuffer simpleConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

struct VertexShaderOutput
{
    float4 pos : POSITION;
    float4 extr : TEXCOORD0;
};

float4 main(float4 pos : POSITION) : SV_POSITION
{
    pos = mul(pos, view);
    pos = mul(pos, projection);
	return pos;
}