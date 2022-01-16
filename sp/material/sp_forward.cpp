#include "sp_forward.h"

#include "../video/sp_video.h"
#include "../debug/sp_cvar.h"

sp_cvar* scene_wireframe;

void sp_forward_init(sp_forward* forward)
{
	scene_wireframe = cvar_registry.get("mat_wireframe");
	
    sp_texture_init(&forward->rtv, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R16G16B16A16_FLOAT, sp_texture_bind::rtv | sp_texture_bind::srv);
    sp_texture_init_rtv(&forward->rtv);
    sp_texture_init_srv(&forward->rtv);
    sp_texture_init_dsv(&forward->rtv, DXGI_FORMAT_D32_FLOAT);
    sp_sampler_init(&forward->texture_sampler, sp_texture_address::mirror);
	
    sp_shader_init(&forward->forward_shader, "data/shaders/forward/forward_vs.hlsl", "data/shaders/forward/forward_ps.hlsl");
}

void sp_forward_free(sp_forward* forward)
{
    sp_shader_free(&forward->forward_shader);
    sp_sampler_free(&forward->texture_sampler);
    sp_texture_free(&forward->rtv);
}

void sp_forward_update(sp_forward* forward, sp_scene* scene, sp_env_map* map)
{
	sp_texture_reset_rtv();
	sp_texture_reset_srv(0, sp_uniform_bind::pixel);
	sp_texture_reset_srv(1, sp_uniform_bind::pixel);
	sp_texture_reset_srv(2, sp_uniform_bind::pixel);
	sp_texture_reset_srv(0, sp_uniform_bind::pixel);
	sp_texture_reset_srv(3, sp_uniform_bind::pixel);
	sp_texture_reset_srv(4, sp_uniform_bind::pixel);
	sp_texture_reset_srv(5, sp_uniform_bind::pixel);
	
    sp_video_data.device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
    sp_texture_bind_rtv(&forward->rtv, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    sp_shader_bind(&forward->forward_shader);
	
    sp_sampler_bind(&forward->texture_sampler, 0, sp_uniform_bind::pixel);
    sp_buffer_bind_cb(&scene->camera_buffer, 1, sp_uniform_bind::vertex);
	sp_buffer_bind_cb(&scene->scene_render_buffer, 2, sp_uniform_bind::pixel);
	
	sp_texture_bind_srv(&map->irradiance_map, 3, sp_uniform_bind::pixel);
	sp_texture_bind_srv(&map->prefilter_map, 4, sp_uniform_bind::pixel);
	sp_texture_bind_srv(&map->brdf_map, 5, sp_uniform_bind::pixel);
	
    for (auto scene_entity : scene->entities)
    {
        sp_entity entity = scene_entity.second;
        if (scene_wireframe->as.i)
			sp_material_bind(&scene->wireframe_material);
		else
			sp_material_bind(&scene->materials[entity.material_name]);
		
        sp_buffer_bind_cb(&entity.gpu_transform, 0, sp_uniform_bind::vertex);
        for (i32 j = 0; j < entity.render_model.meshes.size(); j++)
        {
            sp_mesh mesh = entity.render_model.meshes[j];
			
			sp_texture_reset_srv(0, sp_uniform_bind::pixel);
			sp_texture_reset_srv(1, sp_uniform_bind::pixel);
			sp_texture_reset_srv(2, sp_uniform_bind::pixel);
			
            sp_texture_bind_srv(&mesh.albedo_texture, 0, sp_uniform_bind::pixel);
			sp_texture_bind_srv(&mesh.metallic_roughness_texture, 1, sp_uniform_bind::pixel);
			sp_texture_bind_srv(&mesh.normal_texture, 2, sp_uniform_bind::pixel);
			
            sp_buffer_bind_vb(&mesh.vertex_buffer);
            sp_buffer_bind_ib(&mesh.index_buffer);
            sp_video_draw_indexed(mesh.index_count, 0);
			
            sp_texture_reset_srv(0, sp_uniform_bind::pixel);
			sp_texture_reset_srv(1, sp_uniform_bind::pixel);
			sp_texture_reset_srv(2, sp_uniform_bind::pixel);
        }
    }
	
	sp_texture_reset_srv(3, sp_uniform_bind::pixel);
	sp_texture_reset_srv(4, sp_uniform_bind::pixel);
	sp_texture_reset_srv(5, sp_uniform_bind::pixel);
}

void sp_forward_resize(sp_forward* forward)
{
    sp_texture_free(&forward->rtv);
    sp_texture_init(&forward->rtv, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R8G8B8A8_UNORM, sp_texture_bind::rtv | sp_texture_bind::srv);
    sp_texture_init_rtv(&forward->rtv);
    sp_texture_init_srv(&forward->rtv);
    sp_texture_init_dsv(&forward->rtv, DXGI_FORMAT_D32_FLOAT);
}