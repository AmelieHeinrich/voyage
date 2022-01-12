#include "sp_game.h"

#include "video/sp_video.h"
#include "video/sp_shader.h"
#include "video/sp_buffer.h"
#include "video/sp_mesh.h"
#include "material/sp_material.h"
#include "material/sp_render_flow.h"
#include "sp_log.h"

struct sp_game
{
    sp_render_flow render_flow;
    sp_model cube;
};

sp_game game_state;

void sp_game_init(HWND hwnd)
{
    sp_log_info("Initialised game");

    sp_video_init(hwnd);
    sp_render_flow_init(&game_state.render_flow);

    sp_model_load(&game_state.cube, "data/models/cube.obj");

    game_state.render_flow.update.drawables[0] = game_state.cube;
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