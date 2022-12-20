struct VertexOut
{
    float4 position: SV_POSITION;
    float2 uv: TEXCOORD;
};

VertexOut main(uint vI : SV_VERTEXID)
{
    VertexOut output = (VertexOut)0;

    float2 texcoord = float2(vI&1,vI>>1);

    output.position = float4((texcoord.x-0.5f)*2,-(texcoord.y-0.5f)*2,0,1);
    output.uv = texcoord;

    return output;
}
