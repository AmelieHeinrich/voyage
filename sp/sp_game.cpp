#include "sp_game.h"

#include "sp_timer.h"
#include "video/sp_video.h"
#include "video/sp_shader.h"
#include "video/sp_buffer.h"
#include "video/sp_mesh.h"
#include "material/sp_material.h"
#include "material/sp_render_flow.h"
#include "player/sp_debug_camera.h"
#include "audio/sp_audio.h"
#include "entity/sp_scene.h"
#include "sp_log.h"

#include <glm/gtc/matrix_transform.hpp>

struct sp_game
{
    sp_scene our_scene;
    sp_render_flow render_flow;
    sp_debug_camera cam;
    sp_entity helmet_entity;

    f32 last_frame;
};

sp_game game_state;

void sp_game_init(HWND hwnd)
{
    sp_timer_init();
    sp_audio_init();
    sp_video_init(hwnd);
    sp_scene_init(&game_state.our_scene);
    sp_render_flow_init(&game_state.render_flow);

    //
    sp_material_info mat_info{};
    mat_info.ccw = false;
    mat_info.cull_mode = sp_cull_mode::back;
    mat_info.fill_mode = sp_fill_mode::fill;
    mat_info.depth_op = sp_comp_op::less;
    sp_scene_push_material(&game_state.our_scene, mat_info);

    sp_entity_init(&game_state.helmet_entity, "Helmet");
    sp_entity_load_mesh(&game_state.helmet_entity, "data/models/helmet/DamagedHelmet.gltf");
    sp_entity_load_audio(&game_state.helmet_entity, "data/audio/music.wav");
    sp_entity_set_rotation(&game_state.helmet_entity, 90.0f, 0.0f, 0.0f);
    sp_entity_set_material_index(&game_state.helmet_entity, 0);
    sp_scene_push_entity(&game_state.our_scene, game_state.helmet_entity);

    sp_debug_camera_init(&game_state.cam);

    sp_log_info("Initialised game");
}

void sp_game_shutdown()
{
    sp_scene_free(&game_state.our_scene);
    sp_render_flow_free(&game_state.render_flow);
    sp_video_shutdown();
    sp_audio_free();
}

void sp_game_update()
{
    sp_audio_update();

    f32 time = sp_timer_get();
    f32 dt = time - game_state.last_frame;
    game_state.last_frame = time;

    game_state.our_scene.scene_camera.projection = game_state.cam.projection;
    game_state.our_scene.scene_camera.view = game_state.cam.view;

    sp_scene_update(&game_state.our_scene);
    sp_render_flow_update(&game_state.render_flow, &game_state.our_scene);
    sp_render_flow_render(&game_state.render_flow);

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