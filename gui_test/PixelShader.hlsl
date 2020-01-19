struct PixelShaderInput
{
    float4 pos : SV_POSITION;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
     return float4(0.9098f, 0.7803f, 0.9294f, 1.0f);
}