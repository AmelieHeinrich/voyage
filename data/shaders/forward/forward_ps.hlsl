struct FragmentIn
{
    float4 position: SV_POSITION;
};

float4 main(FragmentIn input) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}