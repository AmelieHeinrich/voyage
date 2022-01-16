#include "sp_fxaa.h"

#include "../video/sp_video.h"
#include "sp_render_flow.h"
#include "../sp_input.h"
#include "../debug/sp_cvar.h"

sp_cvar* fxaa_enabled;

void sp_fxaa_init(sp_fxaa* fxaa)
{
	fxaa_enabled = cvar_registry.get("mat_toggle_fxaa");
	
    sp_shader_init(&fxaa->fxaa_shader, "data/shaders/fxaa/fxaa_vs.hlsl", "data/shaders/fxaa/fxaa_ps.hlsl");
    sp_texture_init(&fxaa->fxaa_texture, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R8G8B8A8_UNORM, sp_texture_bind::rtv | sp_texture_bind::srv);
    sp_texture_init_rtv(&fxaa->fxaa_texture);
    sp_texture_init_srv(&fxaa->fxaa_texture);
    sp_buffer_create(&fxaa->fxaa_parameters, sizeof(fxaa->fxaa_parameters), 0, sp_buffer_usage::uniform);
	
    sp_material_info mat_info{};
    mat_info.ccw = false;
    mat_info.cull_mode = sp_cull_mode::none;
    mat_info.depth_op = sp_comp_op::never;
    mat_info.fill_mode = sp_fill_mode::fill;
    sp_material_create(&fxaa->fxaa_material, mat_info);
    sp_sampler_init(&fxaa->fxaa_sampler, sp_texture_address::wrap);
	
    fxaa->params.fxaa_enabled = true;
    fxaa->params.rcp_frame = glm::vec2(1.0f / sp_video_data.width, 1.0f / sp_video_data.height);    
}

void sp_fxaa_free(sp_fxaa* fxaa)
{
    sp_material_free(&fxaa->fxaa_material);
    sp_sampler_free(&fxaa->fxaa_sampler);
    sp_texture_free(&fxaa->fxaa_texture);
    sp_shader_free(&fxaa->fxaa_shader);
}

void sp_fxaa_update(sp_fxaa* fxaa, sp_texture* in_texture)
{
	fxaa->params.fxaa_enabled = fxaa_enabled->as.i;
	
	sp_texture_reset_rtv();
	sp_texture_reset_srv(0, sp_uniform_bind::pixel);
	
    sp_buffer_set_data(&fxaa->fxaa_parameters, &fxaa->params);
    sp_video_data.device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
    sp_shader_bind(&fxaa->fxaa_shader);
    sp_material_bind(&fxaa->fxaa_material);
    sp_sampler_bind(&fxaa->fxaa_sampler, 0, sp_uniform_bind::pixel);
    sp_texture_bind_rtv(&fxaa->fxaa_texture, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    sp_buffer_bind_cb(&fxaa->fxaa_parameters, 0, sp_uniform_bind::pixel);
    sp_texture_bind_srv(in_texture, 0, sp_uniform_bind::pixel);
    sp_video_draw(4, 0);
}

void sp_fxaa_resize(sp_fxaa* fxaa)
{
    sp_texture_free(&fxaa->fxaa_texture);
    sp_texture_init(&fxaa->fxaa_texture, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R8G8B8A8_UNORM, sp_texture_bind::rtv | sp_texture_bind::srv);
    sp_texture_init_rtv(&fxaa->fxaa_texture);
    sp_texture_init_srv(&fxaa->fxaa_texture);
}
