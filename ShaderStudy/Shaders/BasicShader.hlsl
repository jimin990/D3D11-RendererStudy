// 그릴 물체의 크기 변경을 위한 코드
/*
* 셰이더 코드
* HSLS 셰이더 코드를 문자열로 보관
*/

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer TransformBuffer : register(b0)
{
    float scale;
    float3 padding;
};

struct VSInput
{
    float3 Position : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION; // SV_POSITION 시맨틱은 반환되면, 해당 정점의 Rasterizer가 사용할 최종 위치 값으로 지정한다.
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.Position = float4(input.Position * scale, 1.0f);
    output.uv = input.uv;
    return output;
}

// SV_TARGET 시맨틱이 지정된 셰이더의 반환값은 Rand Target으로 이동한다.
float4 PSMain(VSOutput input) : SV_TARGET
{
    return texture0.Sample(sampler0, input.uv);
}