struct FragmentIn
{
    float4 position: SV_POSITION;
    float3 normal: NORMAL;
    float2 texcoord: TEXCOORD;
};

float4 main(FragmentIn input) : SV_Target
{
    return float4(1.0);
}