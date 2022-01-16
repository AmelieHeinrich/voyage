#include "sp_game.h"
#include "sp_timer.h"
#include "sp_platform.h"
#include "sp_input.h"
#include "video/sp_video.h"
#include "video/sp_shader.h"
#include "video/sp_buffer.h"
#include "video/sp_mesh.h"
#include "material/sp_material.h"
#include "material/sp_gui.h"
#include "audio/sp_audio.h"
#include "script/sp_script_engine.h"
#include "debug/sp_console.h"
#include "debug/sp_cvar.h"
#include "sp_log.h"
#include "sp_timer.h"
#include "sp_platform.h"
#include "sp_input.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "sp_game_gui.h"

sp_game_state* game_state;

sp_game_state* sp_get_game_state()
{
	return game_state;
}

void sp_game_init(HWND hwnd)
{
	game_state = new sp_game_state();
	
	platform_data.vsync = 1;
	game_state->open_console = 0;
	game_state->console_focused = 0;
	
	sp_init_cvar_registry();
    sp_timer_init();
    sp_audio_init();
    sp_video_init(hwnd);
	sp_dev_console_init();
    sp_script_engine_init();
    sp_script_engine_register();
    sp_scene_init(&game_state->current_scene);
    sp_render_flow_init(&game_state->render_flow, hwnd);
	sp_init_game_gui();
	
    //
    sp_material_info mat_info{};
    mat_info.ccw = false;
    mat_info.cull_mode = sp_cull_mode::back;
    mat_info.fill_mode = sp_fill_mode::fill;
    mat_info.depth_op = sp_comp_op::less;
	mat_info.name = "DefaultMeshMaterial";
    sp_scene_push_material(&game_state->current_scene, mat_info);
	
    sp_entity_init(&game_state->helmet_entity, "helmet");
    sp_script_engine_load_script(&game_state->helmet_entity, "lua/helmet.lua");
    sp_scene_push_entity(&game_state->current_scene, game_state->helmet_entity);
	
    sp_debug_camera_init(&game_state->cam);
	
    sp_scene_on_init(&game_state->current_scene);
	
    sp_log_info("[INFO] Initialised game");
}

void sp_game_shutdown()
{
    sp_scene_on_free(&game_state->current_scene);
	
    sp_scene_free(&game_state->current_scene);
    sp_render_flow_free(&game_state->render_flow);
    sp_script_engine_free();
	sp_dev_console_shutdown();
    sp_video_shutdown();
    sp_audio_free();
	sp_free_cvar_registry();
	
	sp_log_info("[INFO] Terminated game");
	
	delete game_state;
}

void sp_game_update()
{
	if (sp_key_pressed(SP_KEY_F1))
		game_state->open_console = true;
	
    sp_audio_update();
	
    f32 time = sp_timer_get();
    f32 dt = time - game_state->last_frame;
    game_state->last_frame = time;
	
    game_state->current_scene.scene_camera.projection = game_state->cam.projection;
    game_state->current_scene.scene_camera.view = game_state->cam.view;
	game_state->current_scene.scene_camera.camera_position = glm::vec4(game_state->cam.position, 1.0f);
	
	sp_texture_reset_srv(0, sp_uniform_bind::pixel);
    sp_scene_on_update(&game_state->current_scene);
    sp_render_flow_update(&game_state->render_flow, &game_state->current_scene);
	
	sp_texture_reset_srv(0, sp_uniform_bind::pixel);
	sp_gui_begin();
	sp_dev_console_draw(&game_state->open_console, &game_state->console_focused);
	sp_showpos();
	sp_gui_end();
	
	sp_render_flow_render(&game_state->render_flow);
    sp_video_present(platform_data.vsync);
	
    if (!game_state->console_focused)
		sp_debug_camera_input(&game_state->cam, dt);
    sp_debug_camera_update(&game_state->cam, dt);
}

void sp_game_resize(u32 width, u32 height)
{
    if (sp_video_data.swap_chain)
    {
        sp_video_resize(width, height);
        sp_render_flow_resize(&game_state->render_flow);
    }
}