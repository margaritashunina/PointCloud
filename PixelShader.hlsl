cbuffer simpleConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

struct PixelShaderInput
{
    float4 pos : POSITION;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 pos = input.pos;
    float dist = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
    //float cap = 1. - dist / 1.26;
    //cap = min(1.0, cap + 0.3);
    float cap = dist / 3;
    cap = max(0.0, cap - 0.01);
    return float4(cap, cap, cap, 1.0);
}