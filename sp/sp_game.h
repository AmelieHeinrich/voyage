#pragma once

#include <Windows.h>
#include "sp_common.h"

#include "material/sp_render_flow.h"
#include "entity/sp_scene.h"
#include "player/sp_debug_camera.h"

struct sp_game_state
{
    sp_scene current_scene;
    sp_render_flow render_flow;
    sp_debug_camera cam;
    sp_entity helmet_entity;
	
	bool open_console;
	bool game_focus;
	bool console_focused;
	bool enable_skybox;
	bool enable_fxaa;
    f32 last_frame;
};

sp_game_state* sp_get_game_state();

void sp_game_init(HWND hwnd);
void sp_game_shutdown();
void sp_game_update();
void sp_game_resize(u32 width, u32 height);