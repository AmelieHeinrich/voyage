// bloom.hlsl: Compute Shader for bloom post processing

cbuffer BloomParameters : register(b0)
{
	bool bloom_enabled;
	float bloom_threshold;
	float2 padding;
};

Texture2D<float4> InTexture : register(t0);
RWTexture2D<float4> OutTexture : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
	float4 out_color = float4(0.1, 0.1, 0.1, 0.1);

	float outputWidth, outputHeight;
	InTexture.GetDimensions(outputWidth, outputHeight);

	if (ThreadID.x >= uint(outputWidth) || ThreadID.y >= uint(outputHeight))
		return;	

	uint2 pixel_pos = uint2(ThreadID.x, ThreadID.y);
	float4 color = InTexture[pixel_pos];

	if (bloom_enabled)
	{
		float brightness = (color.r * 0.2126) + (color.g * 0.7152) + (color.b * 0.0722);
		out_color = color * brightness * bloom_threshold;
	}
	else
		out_color = color;

	OutTexture[pixel_pos] = out_color;
}