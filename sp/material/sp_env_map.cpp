#include "sp_env_map.h"

#include <glm/gtc/matrix_transform.hpp>
#include "../video/sp_video.h"

#include "../sp_log.h"
#include "../sp_timer.h"

const float skybox_vertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

void sp_env_map_init(sp_env_map* map, const char* path)
{
    // TODO (milo): make the compute execution time faster
    
    map->path = (char*)path;
    sp_log_info("[INFO] Loading environment map from path %s", path);
    
    sp_shader_init(&map->equi_shader, NULL, NULL, "data/shaders/env_map/equi.hlsl");
    sp_shader_init(&map->irradiance_shader, NULL, NULL, "data/shaders/env_map/irr.hlsl");
    sp_shader_init(&map->prefilter_shader, NULL, NULL, "data/shaders/env_map/pre.hlsl");
    sp_shader_init(&map->brdf_shader, NULL, NULL, "data/shaders/env_map/brdf.hlsl");
    sp_shader_init(&map->draw_shader, "data/shaders/env_map/draw_v.hlsl", "data/shaders/env_map/draw_p.hlsl");
    
    sp_texture_load_float(&map->equi_map, path);
    
    sp_texture_init_cube(&map->cube_map, 512, 512, DXGI_FORMAT_R16G16B16A16_UNORM, sp_texture_bind::srv | sp_texture_bind::uav);
    sp_texture_init_srv_cube(&map->cube_map);
    sp_texture_init_uav_cube(&map->cube_map);
    
    sp_texture_init_cube(&map->irradiance_map, 128, 128, DXGI_FORMAT_R16G16B16A16_UNORM,
                         sp_texture_bind::srv | sp_texture_bind::uav);
    sp_texture_init_srv_cube(&map->irradiance_map);
    sp_texture_init_uav_cube(&map->irradiance_map);
    
    sp_texture_init_cube(&map->prefilter_map, 512, 512, DXGI_FORMAT_R16G16B16A16_UNORM,
                         sp_texture_bind::srv | sp_texture_bind::uav);
    sp_texture_init_srv_cube(&map->prefilter_map);
    sp_texture_init_uav_cube(&map->prefilter_map);
    
    sp_texture_init(&map->brdf_map, 512, 512, DXGI_FORMAT_R16G16B16A16_UNORM,
                    sp_texture_bind::srv | sp_texture_bind::uav);
    sp_texture_init_srv(&map->brdf_map);
    sp_texture_init_uav(&map->brdf_map);
    
    sp_sampler_init(&map->cube_map_sampler, sp_texture_address::wrap);
    
    sp_material_info mat_info{};
    mat_info.fill_mode = sp_fill_mode::fill;
    mat_info.cull_mode = sp_cull_mode::none;
    mat_info.depth_op = sp_comp_op::less_equal;
    mat_info.ccw = false;
    sp_material_create(&map->cube_material, mat_info);
    
    sp_buffer_create(&map->cube_map_buffer, sizeof(skybox_vertices), sizeof(glm::vec3), sp_buffer_usage::vertex);
    sp_buffer_create(&map->cube_upd_buffer, sizeof(glm::mat4), 0, sp_buffer_usage::uniform);
    sp_buffer_set_data(&map->cube_map_buffer, (void*)skybox_vertices);
    
    // COMPUTE 
    f32 start = sp_timer_get();
    
    // Equi to cubemap
    sp_texture_reset_srv(0, sp_uniform_bind::compute);
    sp_shader_bind(&map->equi_shader);
    sp_sampler_bind(&map->cube_map_sampler, 0, sp_uniform_bind::compute);
    sp_texture_bind_srv(&map->equi_map, 0, sp_uniform_bind::compute);
    sp_texture_bind_uav(&map->cube_map, 0);
    
    sp_video_dispatch(1024 / 32, 1024 / 32, 6);
    
    sp_texture_reset_srv(0, sp_uniform_bind::compute);
    sp_texture_reset_uav(0);
    sp_texture_free(&map->equi_map);
    
    // Irradiance
    sp_texture_reset_srv(0, sp_uniform_bind::compute);
    sp_shader_bind(&map->irradiance_shader);
    sp_sampler_bind(&map->cube_map_sampler, 0, sp_uniform_bind::compute);
    sp_texture_bind_srv(&map->cube_map, 0, sp_uniform_bind::compute);
    sp_texture_bind_uav(&map->irradiance_map, 0);
    
    sp_video_dispatch(128 / 32, 128 / 32, 6);
    
    sp_texture_reset_srv(0, sp_uniform_bind::compute);
    sp_texture_reset_uav(0);
    
    // Prefilter
    sp_buffer prefilter_buffer;
    sp_buffer_create(&prefilter_buffer, sizeof(glm::vec4), 0, sp_buffer_usage::uniform);
    
    sp_texture_reset_srv(0, sp_uniform_bind::compute);
    sp_shader_bind(&map->prefilter_shader);
    sp_sampler_bind(&map->cube_map_sampler, 0, sp_uniform_bind::compute);
    sp_texture_bind_srv(&map->cube_map, 0, sp_uniform_bind::compute);
    sp_texture_bind_uav(&map->prefilter_map, 0);
    sp_buffer_bind_cb(&prefilter_buffer, 0, sp_uniform_bind::compute);
    
    for (u32 i = 0; i < 5; i++)
    {
        u32 mip_width = (u32)(512.0f * pow(0.5f, i));
        u32 mip_height = (u32)(512.0f * pow(0.5f, i));
        f32 roughness = (f32)i / (f32)(5 - 1);
        
        glm::vec4 roughness_vector(0.0f);
        roughness_vector.x = roughness;
        
        sp_buffer_set_data(&prefilter_buffer, &roughness_vector);
        sp_video_dispatch(mip_width / 32, mip_height / 32, 6);
    }
    
    sp_texture_reset_srv(0, sp_uniform_bind::compute);
    sp_texture_reset_uav(0);
    sp_buffer_free(&prefilter_buffer);
    
    // BRDF
    sp_shader_bind(&map->brdf_shader);
    sp_sampler_bind(&map->cube_map_sampler, 0, sp_uniform_bind::compute);
    sp_texture_bind_uav(&map->brdf_map, 0);
    sp_video_dispatch(512 / 32, 512 / 32, 1);
    
    sp_texture_reset_uav(0);
    //
    
    f32 end = sp_timer_get();
    sp_log_info("[INFO] Environment map compute took %f seconds", end - start);
    //
}

void sp_env_map_free(sp_env_map* map)
{
    sp_buffer_free(&map->cube_upd_buffer);
    sp_buffer_free(&map->cube_map_buffer);
    
    sp_texture_free(&map->brdf_map);
    sp_texture_free(&map->prefilter_map);
    sp_texture_free(&map->irradiance_map);
    sp_sampler_free(&map->cube_map_sampler);
    sp_texture_free(&map->cube_map);
    
    sp_material_free(&map->cube_material);
    sp_shader_free(&map->draw_shader);
    sp_shader_free(&map->brdf_shader);
    sp_shader_free(&map->prefilter_shader);
    sp_shader_free(&map->irradiance_shader);
    sp_shader_free(&map->equi_shader);
}

void sp_env_map_update(sp_env_map* map, sp_scene* scene)
{	
    glm::mat4 mvp(1.0f);
    
    mvp = scene->scene_camera.projection
        * glm::mat4(glm::mat3(scene->scene_camera.view)) 
        * glm::scale(glm::mat4(1.0f), glm::vec3(10000.0f, 10000.0f, 10000.0f));
    sp_buffer_set_data(&map->cube_upd_buffer, &mvp);
    
    sp_shader_bind(&map->draw_shader);
    sp_material_bind(&map->cube_material);
    sp_sampler_bind(&map->cube_map_sampler, 0, sp_uniform_bind::pixel);
    sp_texture_reset_srv(0, sp_uniform_bind::pixel);
    sp_texture_bind_srv(&map->cube_map, 0, sp_uniform_bind::pixel);
    sp_buffer_bind_cb(&map->cube_upd_buffer, 0, sp_uniform_bind::vertex);
    sp_buffer_bind_vb(&map->cube_map_buffer);
    sp_video_draw(36, 0);
}

void sp_env_map_restart(sp_env_map* map, char* path)
{
    
}