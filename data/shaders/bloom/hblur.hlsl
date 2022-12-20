// hblur.hlsl: Horizontal blur compute shader

cbuffer Parameters : register(b0)
{
	int radius;
	float3 padding;
};

Texture2D<float4> InTexture : register(t0);
RWTexture2D<float4> OutTexture : register(u0);

static const float PI = 3.141592653589793;
static const float sigma = 3.0f;

float4 ComputeGaussianBlur(uint2 ThreadID, float width, float height)
{
	float two_sigma_squared = 2 * sigma * sigma;
	int r = radius;	
	float all_weight = 0.0;
	float4 color = float4(0.0, 0.0, 0.0, 0.0);
	for (int x = -r; x < (r + 1); x++)
	{
		for (int y = -r; y < (r + 1); y++)
		{
			float weight = 1.0f / (PI * two_sigma_squared) * exp(-(x * x + y * y) / two_sigma_squared);
			float2 offset = float2(1.0f / width * x, 1.0 / height * y);
			float2 uv_offset = ThreadID + offset;
			color += InTexture[uv_offset] * weight;
			all_weight += weight;
		}
	}
	color /= all_weight;
	return color;
}