struct VertexIn
{
    float3 position: POSITION;
    float3 color: COLOR;
};

struct VertexOut
{
    float4 position: SV_POSITION;
    float3 color: COLOR;
};

VertexOut main(VertexIn input)
{
    VertexOut output = (VertexOut)0;
    
    output.position = float4(input.position, 1.0);
    output.color = input.color;

    return output;
}