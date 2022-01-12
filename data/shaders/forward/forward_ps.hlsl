struct FragmentIn
{
    float4 position: SV_POSITION;
    float3 color: COLOR;
};

float4 main(FragmentIn input) : SV_Target
{
    return float4(input.color, 1.0);
}