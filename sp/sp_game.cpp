#include "sp_game.h"

#include "video/sp_video.h"
#include "video/sp_shader.h"
#include "sp_log.h"

struct sp_game
{
    sp_shader forward_shader;
};

sp_game game_state;

void sp_game_init(HWND hwnd)
{
    sp_log_info("Initialised game");

    sp_video_init(hwnd);
    sp_shader_init(&game_state.forward_shader, "data/shaders/forward/forward_vs.hlsl", "data/shaders/forward/forward_ps.hlsl");
}

void sp_game_shutdown()
{
    sp_shader_free(&game_state.forward_shader);
    sp_video_shutdown();
}

void sp_game_update()
{
    sp_video_begin();
    sp_video_present(1);
}

void sp_game_resize(u32 width, u32 height)
{
    if (sp_video_data.swap_chain)
        sp_video_resize(width, height);
}