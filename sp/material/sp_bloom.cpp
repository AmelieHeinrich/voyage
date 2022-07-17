#include "sp_bloom.h"

#include "../video/sp_video.h"
#include "sp_render_flow.h"
#include "../sp_input.h"
#include "../debug/sp_cvar.h"

sp_cvar* bloom_enabled;
sp_cvar* bloom_threshold;

void sp_bloom_init(sp_bloom* bloom)
{
	bloom_enabled = cvar_registry.get("mat_toggle_bloom");
	bloom_threshold = cvar_registry.get("mat_bloom_threshold");
	
	sp_shader_init(&bloom->bloom_shader, NULL, NULL, "data/shaders/bloom/bloom.hlsl");
	sp_shader_init(&bloom->vertical_blur_shader, NULL, NULL, "data/shaders/bloom/vblur.hlsl");
	sp_shader_init(&bloom->horizontal_blur_shader, NULL, NULL, "data/shaders/bloom/hblur.hlsl");
	//sp_shader_init(&bloom->combine_shader, NULL, NULL, "data/shaders/bloom/combine.hlsl");
	
	sp_texture_init(&bloom->bright_texture, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R16G16B16A16_UNORM, sp_texture_bind::srv | sp_texture_bind::uav);
	sp_texture_init(&bloom->blur_texture, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R16G16B16A16_UNORM, sp_texture_bind::srv | sp_texture_bind::uav);
	sp_texture_init(&bloom->combine_texture, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R16G16B16A16_UNORM, sp_texture_bind::srv | sp_texture_bind::uav);
	sp_texture_init_srv(&bloom->bright_texture);
	sp_texture_init_uav(&bloom->bright_texture);
	sp_texture_init_srv(&bloom->blur_texture);
	sp_texture_init_uav(&bloom->blur_texture);
	sp_texture_init_srv(&bloom->combine_texture);
	sp_texture_init_uav(&bloom->combine_texture);
	sp_sampler_init(&bloom->bloom_sampler, sp_texture_address::wrap);
	sp_buffer_create(&bloom->bloom_parameters, sizeof(bloom->params), 0, sp_buffer_usage::uniform);
	sp_buffer_create(&bloom->blur_buffer, sizeof(float) * 9, 0, sp_buffer_usage::uniform);
	
	bloom->params.bloom_enabled = bloom_enabled->as.i;
	bloom->params.bloom_threshold = bloom_threshold->as.f;
}

void sp_bloom_free(sp_bloom* bloom)
{
	sp_texture_free(&bloom->combine_texture);
	sp_texture_free(&bloom->blur_texture);
	sp_texture_free(&bloom->bright_texture);
	sp_buffer_free(&bloom->bloom_parameters);
	sp_buffer_free(&bloom->blur_buffer);
	sp_sampler_free(&bloom->bloom_sampler);
	//sp_shader_free(&bloom->combine_shader);
	sp_shader_free(&bloom->horizontal_blur_shader);
	sp_shader_free(&bloom->vertical_blur_shader);
	sp_shader_free(&bloom->bloom_shader);
}

void sp_bloom_set_weights(sp_bloom* bloom)
{
	const float sigma = 2.2f;
	
	float d = 2.0f*sigma*sigma;
	
	float weights[9];
	float sum = 0.0f;
	for(int i = 0; i < 8; ++i)
	{
		float x = (float)i;
		weights[i] = expf(-x*x/d);
		
		sum += weights[i];
	}
	
	// Divide by the sum so all the weights add up to 1.0.
	for(int i = 0; i < 8; ++i)
	{
		weights[i] /= sum;
	}
	
	sp_buffer_set_data(&bloom->blur_buffer, weights);
}

void sp_bloom_update(sp_bloom* bloom, sp_texture* in_texture)
{
	bloom->params.bloom_enabled = bloom_enabled->as.i;
	bloom->params.bloom_threshold = bloom_threshold->as.f;
	
	sp_texture_reset_uav(0);
	sp_texture_reset_srv(0, sp_uniform_bind::compute);
	sp_texture_reset_rtv();
	
	sp_buffer_set_data(&bloom->bloom_parameters, &bloom->params);
	
	sp_shader_bind(&bloom->bloom_shader);
	sp_texture_bind_srv(in_texture, 0, sp_uniform_bind::compute);
	sp_texture_bind_uav(&bloom->bright_texture, 0);
	sp_buffer_bind_cb(&bloom->bloom_parameters, 0, sp_uniform_bind::compute);
	sp_sampler_bind(&bloom->bloom_sampler, 0, sp_uniform_bind::compute);
	sp_video_dispatch(sp_video_data.width / 32, sp_video_data.height / 32, 1);
	
	sp_texture_reset_uav(0);
	sp_texture_reset_srv(0, sp_uniform_bind::compute);
	sp_texture_reset_rtv();
	
	sp_bloom_set_weights(bloom);
	sp_shader_bind(&bloom->horizontal_blur_shader);
	sp_texture_bind_srv(&bloom->bright_texture, 0, sp_uniform_bind::compute);
	sp_texture_bind_uav(&bloom->blur_texture, 0);
	sp_buffer_bind_cb(&bloom->blur_buffer, 0, sp_uniform_bind::compute);
	u32 num_groups_x = (u32)ceilf(sp_video_data.width / 256.0f);
	sp_video_dispatch(num_groups_x, sp_video_data.height, 1);
	
	sp_texture_reset_uav(0);
	sp_texture_reset_srv(0, sp_uniform_bind::compute);
	sp_texture_reset_rtv();
	
	sp_bloom_set_weights(bloom);
	sp_shader_bind(&bloom->vertical_blur_shader);
	sp_texture_bind_srv(&bloom->blur_texture, 0, sp_uniform_bind::compute);
	sp_texture_bind_uav(&bloom->bright_texture, 0);
	sp_buffer_bind_cb(&bloom->blur_buffer, 0, sp_uniform_bind::compute);
	u32 num_groups_y = (u32)ceilf(sp_video_data.height / 256.0f);
	sp_video_dispatch(sp_video_data.width, num_groups_y, 1);
	
	sp_texture_reset_uav(0);
	sp_texture_reset_srv(0, sp_uniform_bind::compute);
	sp_texture_reset_rtv();
	
	/*
	sp_texture_reset_srv(0, sp_uniform_bind::compute);
	sp_texture_reset_srv(1, sp_uniform_bind::compute);
	sp_texture_reset_uav(0);
	
	sp_shader_bind(&bloom->combine_shader);
	sp_texture_bind_srv(&bloom->bright_texture, 0, sp_uniform_bind::compute);
	sp_texture_bind_srv(in_texture, 1, sp_uniform_bind::compute);
	sp_texture_bind_uav(&bloom->combine_texture, 0);
	sp_sampler_bind(&bloom->bloom_sampler, 0, sp_uniform_bind::compute);
	sp_video_dispatch(sp_video_data.width / 32, sp_video_data.height / 32, 1);*/
}

void sp_bloom_resize(sp_bloom* bloom)
{
	sp_texture_free(&bloom->combine_texture);
	sp_texture_free(&bloom->blur_texture);
	sp_texture_free(&bloom->bright_texture);
	
	sp_texture_init(&bloom->bright_texture, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R16G16B16A16_UNORM, sp_texture_bind::srv | sp_texture_bind::uav);
	sp_texture_init(&bloom->blur_texture, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R16G16B16A16_UNORM, sp_texture_bind::srv | sp_texture_bind::uav);
	sp_texture_init(&bloom->combine_texture, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R16G16B16A16_UNORM, sp_texture_bind::srv | sp_texture_bind::uav);
	sp_texture_init_srv(&bloom->bright_texture);
	sp_texture_init_uav(&bloom->bright_texture);
	sp_texture_init_srv(&bloom->blur_texture);
	sp_texture_init_uav(&bloom->blur_texture);
	sp_texture_init_srv(&bloom->combine_texture);
	sp_texture_init_uav(&bloom->combine_texture);
}