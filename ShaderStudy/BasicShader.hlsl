// 그릴 물체의 크기 변경을 위한 코드
/*
* 셰이더 코드
* HSLS 셰이더 코드를 문자열로 보관
*/

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct VSInput
{
    float3 Position : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.Position = float4(input.Position, 1.0f);
    output.uv = input.uv;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return texture0.Sample(sampler0, input.uv);
}