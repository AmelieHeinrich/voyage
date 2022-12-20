static const float PI = 3.14159265359;

struct FragmentIn
{
    float4 position: SV_POSITION;
    float3 normal: NORMAL;
    float2 texcoord: TEXCOORD;
	float3 tangent: TANGENT;
	float3 bitangent: BITANGENT;
	float4 WorldPos: COLOR0;
	float4 cameraPosition: COLOR1;
};

sampler TextureSampler : register(s0);
Texture2D Albedo : register(t0);
Texture2D MetallicRoughness : register(t1);
Texture2D Normal : register(t2);
TextureCube Irradiance : register(t3);
TextureCube Prefilter : register(t4);
Texture2D BRDF : register(t5);

cbuffer SceneRender : register(b2)
{
    int render_mode;
	bool force_wireframe;
	float2 padding0;
};

cbuffer SceneLights : register(b3)
{
	float4 light_positions[128];
	float4 light_colors[128];
};

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 GetNormalFromMap(FragmentIn input)
{
    float3 tangentNormal = Normal.Sample(TextureSampler, input.texcoord).rgb * 2.0 - 1.0;

    float3 Q1  = ddx(input.WorldPos.xyz);
    float3 Q2  = ddy(input.WorldPos.xyz);
    float2 st1 = ddx(input.texcoord);
    float2 st2 = ddy(input.texcoord);

    float3 N   = normalize(input.normal);
    float3 T  = normalize(Q1*st2.y - Q2*st1.y);
    float3 B  = -normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);

    return normalize(mul(TBN, tangentNormal));
}

static const float MAX_REFLECTION_LOD = 4.0;

float4 PBR(FragmentIn input)
{
	float3 albedo = Albedo.Sample(TextureSampler, input.texcoord).rgb;
	float4 mr_texture = MetallicRoughness.Sample(TextureSampler, input.texcoord);
	float metallic = mr_texture.b;
	float roughness = mr_texture.g;
	float ao = 1.0f;

	float3 normal = input.normal * Normal.Sample(TextureSampler, input.texcoord).rgb;

	float3 N = normalize(normal);
	float3 V = normalize(input.cameraPosition - input.WorldPos).xyz;
	float3 R = reflect(-V, N);

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);

	float3 Lo = float3(0.0, 0.0, 0.0);

	for (int i = 0; i < 128; i++)
	{
		float3 light_pos = light_positions[i].xyz;
		float3 light_color = light_colors[i].xyz;

		float3 L = normalize(light_pos - input.WorldPos.xyz);
		float3 H = normalize(V + L);
		float distance = length(L);
		float attenuation = 1.0 / (distance * distance);
		float3 radiance = light_color * attenuation;
	
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

		float3 numerator = F * G * NDF;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		float3 specular = numerator / max(denominator, 0.001);

		float3 kS = F;
		float3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;
	
		float NdotL = max(dot(N, L), 0.0);	
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	float3 kS = F;
	float3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	
	float3 irradiance = Irradiance.Sample(TextureSampler, N).rgb;
	float3 diffuse = irradiance * albedo;
	
	float prefilter_lod = roughness * MAX_REFLECTION_LOD;
	float3 prefiltered_color = Prefilter.SampleLevel(TextureSampler, R, prefilter_lod).rgb;
	float2 brdf = BRDF.Sample(TextureSampler, float2(max(dot(N, V), 0.0), roughness)).xy;
	float3 specular = prefiltered_color * (F * brdf.x + brdf.y);

	float3 ambient = (kD * diffuse + specular) * ao;
	float3 color = ambient + Lo;

	return float4(color, 1.0);
}

float4 Diffuse(FragmentIn input)
{
	float3 albedo = Albedo.Sample(TextureSampler, input.texcoord).rgb;
	float3 N = normalize(input.normal * Normal.Sample(TextureSampler, input.texcoord).rgb).rgb;
	float3 irradiance = Irradiance.Sample(TextureSampler, N).rgb;
	float3 diffuse = irradiance * albedo;
	return float4(diffuse, 1.0);
}

float4 Specular(FragmentIn input)
{
	float3 albedo = Albedo.Sample(TextureSampler, input.texcoord).rgb;
	float4 mr_texture = MetallicRoughness.Sample(TextureSampler, input.texcoord);
	float metallic = mr_texture.b;
	float roughness = mr_texture.g;

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);

	float3 N = normalize(input.normal * Normal.Sample(TextureSampler, input.texcoord).rgb).rgb;
	float3 V = normalize(input.cameraPosition - input.WorldPos).xyz;
	float3 R = reflect(-V, N);
	float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	float prefilter_lod = roughness * MAX_REFLECTION_LOD;
	float3 prefiltered_color = Prefilter.SampleLevel(TextureSampler, R, prefilter_lod).rgb;
	float2 brdf = BRDF.Sample(TextureSampler, float2(max(dot(N, V), 0.0), roughness)).xy;
	float3 specular = prefiltered_color * (F * brdf.x + brdf.y);
	return float4(specular, 1.0);
}

float4 main(FragmentIn input) : SV_Target
{
	float4 final_color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	switch (render_mode)
	{
	case 0:
		final_color = PBR(input);
		break;
	case 1:
		final_color = Albedo.Sample(TextureSampler, input.texcoord);
		break;
	case 2:
		final_color = MetallicRoughness.Sample(TextureSampler, input.texcoord);
		break;
	case 3:
		float3 normal = input.normal * Normal.Sample(TextureSampler, input.texcoord).rgb;
		float3 N = normalize(normal);
		final_color = float4(N, 1.0);
		break;
	case 4:
		final_color = Diffuse(input);
		break;
	case 5:
		final_color = Specular(input);
		break;
	case 6:
		final_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}

	return final_color;
}