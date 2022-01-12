struct VertexIn
{
    float4 position: POSITION;
};

struct VertexOut
{
    float4 position: SV_POSITION;
};

VertexOut main(VertexIn input)
{
    VertexOut output = (VertexOut)0;
    
    output.position = input.position;

    return output;
}