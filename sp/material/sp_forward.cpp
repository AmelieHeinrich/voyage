#include "sp_forward.h"

#include "../video/sp_video.h"

void sp_forward_init(sp_forward* forward)
{
    sp_shader_init(&forward->forward_shader, "data/shaders/forward/forward_vs.hlsl", "data/shaders/forward/forward_ps.hlsl");
}

void sp_forward_free(sp_forward* forward)
{
    sp_shader_free(&forward->forward_shader);
}

void sp_forward_update(sp_forward* forward, sp_render_update update)
{
    sp_shader_bind(&forward->forward_shader);
    for (i32 i = 0; i < update.drawable_count; i++)
    {
        sp_entity entity = update.drawables[i];
        sp_buffer_bind_cb(&entity.gpu_transform, 0, sp_uniform_bind::vertex);
        for (i32 j = 0; j < entity.model.meshes.size(); j++)
        {
            sp_mesh mesh = entity.model.meshes[j];
            sp_buffer_bind_vb(&mesh.vertex_buffer);
            sp_buffer_bind_ib(&mesh.index_buffer);
            sp_video_draw_indexed(mesh.index_count, 0);
        }
    }
}