#include "sp_forward.h"

#include "../video/sp_video.h"

void sp_forward_init(sp_forward* forward)
{
    sp_texture_init(&forward->rtv, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R8G8B8A8_UNORM, sp_texture_bind::rtv);
    sp_texture_init_rtv(&forward->rtv);
    sp_texture_init_dsv(&forward->rtv, DXGI_FORMAT_D32_FLOAT);

    sp_shader_init(&forward->forward_shader, "data/shaders/forward/forward_vs.hlsl", "data/shaders/forward/forward_ps.hlsl");
}

void sp_forward_free(sp_forward* forward)
{
    sp_shader_free(&forward->forward_shader);
    sp_texture_free(&forward->rtv);
}

void sp_forward_update(sp_forward* forward, sp_render_update update)
{
    sp_video_data.device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    sp_texture_bind_rtv(&forward->rtv, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    sp_shader_bind(&forward->forward_shader);
    sp_buffer_bind_cb(&update.scene_buffer, 1, sp_uniform_bind::vertex);
    for (i32 i = 0; i < update.drawable_count; i++)
    {
        sp_entity entity = update.drawables[i];
        sp_material_bind(&update.materials[entity.mat_index]);
        sp_buffer_bind_cb(&entity.gpu_transform, 0, sp_uniform_bind::vertex);
        for (i32 j = 0; j < entity.model.meshes.size(); j++)
        {
            sp_mesh mesh = entity.model.meshes[j];
            sp_texture_bind_srv(&mesh.albedo_texture, 0, sp_uniform_bind::pixel);
            sp_buffer_bind_vb(&mesh.vertex_buffer);
            sp_buffer_bind_ib(&mesh.index_buffer);
            sp_video_draw_indexed(mesh.index_count, 0);
        }
    }
}

void sp_forward_resize(sp_forward* forward)
{
    sp_texture_free(&forward->rtv);
    sp_texture_init(&forward->rtv, sp_video_data.width, sp_video_data.height, DXGI_FORMAT_R8G8B8A8_UNORM, sp_texture_bind::rtv);
    sp_texture_init_rtv(&forward->rtv);
    sp_texture_init_dsv(&forward->rtv, DXGI_FORMAT_D32_FLOAT);
}