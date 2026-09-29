// 그릴 물체의 크기 변경을 위한 코드
/*
* 셰이더 코드
* HSLS 셰이더 코드를 문자열로 보관
*/
struct VSInput
{
    float3 Position : POSITION;
    float3 color : COLOR;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.Position = float4(input.Position, 1.0f);
    output.color = input.color;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}