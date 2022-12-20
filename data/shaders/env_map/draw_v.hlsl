// draw_v.hlsl: Vertex shader for the environment map

struct VertexIn
{
    float3 Position: POSTIION;
};

struct VertexOut
{
	float4 Position : SV_POSITION;
	float3 LocalPosition : COLOR0;
};

cbuffer Transform : register(b0)
{
	row_major float4x4 ModelViewProjection;
};

VertexOut main(VertexIn input)
{
	VertexOut output = (VertexOut)0;
	output.Position = mul(float4(input.Position, 1.0), ModelViewProjection);
	output.LocalPosition = input.Position;
	return output;
}