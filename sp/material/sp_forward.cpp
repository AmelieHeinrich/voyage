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
        sp_drawable drawable = update.drawables[i];
        sp_material_bind(&drawable.material);
        sp_buffer_bind_vb(&drawable.vertex_buffer);
        sp_buffer_bind_ib(&drawable.index_buffer);
        sp_video_draw_indexed(drawable.index_count, 0);
    }
}