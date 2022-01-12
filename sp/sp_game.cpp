#include "sp_game.h"

#include "sp_timer.h"
#include "video/sp_video.h"
#include "video/sp_shader.h"
#include "video/sp_buffer.h"
#include "video/sp_mesh.h"
#include "material/sp_material.h"
#include "material/sp_render_flow.h"
#include "player/sp_debug_camera.h"
#include "sp_log.h"

#include <glm/gtc/matrix_transform.hpp>

struct sp_game
{
    sp_render_flow render_flow;
    sp_debug_camera cam;
    sp_model cube;

    f32 last_frame;
};

sp_game game_state;

void sp_game_init(HWND hwnd)
{
    sp_timer_init();
    sp_video_init(hwnd);
    sp_render_flow_init(&game_state.render_flow);

    sp_model_load(&game_state.cube, "data/models/sphere.gltf");

    game_state.render_flow.update.drawables[0].model = game_state.cube;
    game_state.render_flow.update.drawables[0].transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    sp_buffer_create(&game_state.render_flow.update.drawables[0].gpu_transform, sizeof(glm::mat4), 0, sp_buffer_usage::uniform);
    sp_buffer_set_data(&game_state.render_flow.update.drawables[0].gpu_transform, &game_state.render_flow.update.drawables[0].transform);
    game_state.render_flow.update.drawable_count++;

    sp_debug_camera_init(&game_state.cam);

    sp_log_info("Initialised game");
}

void sp_game_shutdown()
{
    sp_buffer_free(&game_state.render_flow.update.drawables[0].gpu_transform);
    sp_render_flow_free(&game_state.render_flow);
    sp_video_shutdown();
}

void sp_game_update()
{
    f32 time = sp_timer_get();
    f32 dt = time - game_state.last_frame;
    game_state.last_frame = time;

    game_state.render_flow.update.camera.projection = game_state.cam.projection;
    game_state.render_flow.update.camera.view = game_state.cam.view;

    sp_render_flow_update(&game_state.render_flow);
    sp_render_flow_render(&game_state.render_flow);

    sp_video_begin();
    sp_video_present(1);

    sp_debug_camera_input(&game_state.cam, dt);
    sp_debug_camera_update(&game_state.cam, dt);
}

void sp_game_resize(u32 width, u32 height)
{
    if (sp_video_data.swap_chain)
    {
        sp_video_resize(width, height);
        sp_render_flow_resize(&game_state.render_flow);
    }
}