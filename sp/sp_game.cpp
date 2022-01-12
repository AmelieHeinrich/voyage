#include "sp_game.h"

#include "video/sp_video.h"
#include "video/sp_shader.h"
#include "video/sp_buffer.h"
#include "material/sp_material.h"
#include "material/sp_render_flow.h"
#include "sp_log.h"

struct sp_game
{
    sp_buffer vertex_buffer;
    sp_material tri_mat;
    sp_render_flow render_flow;
};

sp_game game_state;

void sp_game_init(HWND hwnd)
{
    sp_log_info("Initialised game");

    sp_video_init(hwnd);
    sp_render_flow_init(&game_state.render_flow);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    i64 vertex_stride = sizeof(float) * 6;
    sp_buffer_create(&game_state.vertex_buffer, sizeof(vertices), vertex_stride, sp_buffer_usage::vertex);
    sp_buffer_set_data(&game_state.vertex_buffer, vertices);

    sp_material_info mat_info;
    mat_info.cull_mode = sp_cull_mode::back;
    mat_info.fill_mode = sp_fill_mode::fill;
    mat_info.ccw = false;
    sp_material_create(&game_state.tri_mat, mat_info);

    game_state.render_flow.update.drawables[0].material = game_state.tri_mat;
    game_state.render_flow.update.drawables[0].vertex_buffer = game_state.vertex_buffer;
    game_state.render_flow.update.drawables[0].vertex_count = 3;
    game_state.render_flow.update.drawable_count++;
}

void sp_game_shutdown()
{
    sp_render_flow_free(&game_state.render_flow);
    sp_video_shutdown();
}

void sp_game_update()
{
    sp_video_begin();
    sp_render_flow_update(&game_state.render_flow);
    sp_video_present(1);
}

void sp_game_resize(u32 width, u32 height)
{
    if (sp_video_data.swap_chain)
        sp_video_resize(width, height);
}