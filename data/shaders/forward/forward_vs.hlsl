struct VertexIn
{
    float3 position: POSITION;
    float3 normal: NORMAL;
    float2 texcoord: TEXCOORD;
};

struct VertexOut
{
    float4 position: SV_POSITION;
    float3 normal: NORMAL;
    float2 texcoord: TEXCOORD;
};

cbuffer ObjectTransform : register(b0)
{
    row_major float4x4 Model;
};

cbuffer SceneData : register(b1)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
};

VertexOut main(VertexIn input)
{
    VertexOut output = (VertexOut)0;
    
    output.position = mul(float4(input.position, 1.0), Model);
    output.position = mul(output.position, Projection);
    output.position = mul(output.position, View);
    output.normal = input.normal;
    output.texcoord = input.texcoord;

    return output;
}