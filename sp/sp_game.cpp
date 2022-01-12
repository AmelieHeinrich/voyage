#include "sp_game.h"

#include "video/sp_video.h"
#include "video/sp_shader.h"
#include "video/sp_buffer.h"
#include "video/sp_mesh.h"
#include "material/sp_material.h"
#include "material/sp_render_flow.h"
#include "sp_log.h"

#include <glm/gtc/matrix_transform.hpp>

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

    sp_model_load(&game_state.cube, "data/models/sphere.gltf");

    game_state.render_flow.update.drawables[0].model = game_state.cube;
    game_state.render_flow.update.drawables[0].transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    sp_buffer_create(&game_state.render_flow.update.drawables[0].gpu_transform, sizeof(glm::mat4), 0, sp_buffer_usage::uniform);
    sp_buffer_set_data(&game_state.render_flow.update.drawables[0].gpu_transform, &game_state.render_flow.update.drawables[0].transform);
    game_state.render_flow.update.drawable_count++;

    game_state.render_flow.update.camera.projection = glm::perspective(90.0f, 1280.0f / 720.0f, 0.001f, 1000.0f);
    game_state.render_flow.update.camera.view = glm::mat4(1.0f);
}

void sp_game_shutdown()
{
    sp_buffer_free(&game_state.render_flow.update.drawables[0].gpu_transform);
    sp_render_flow_free(&game_state.render_flow);
    sp_video_shutdown();
}

void sp_game_update()
{
    sp_render_flow_update(&game_state.render_flow);
    sp_render_flow_render(&game_state.render_flow);

    sp_video_begin();
    sp_video_present(1);
}

void sp_game_resize(u32 width, u32 height)
{
    if (sp_video_data.swap_chain)
        sp_video_resize(width, height);
}