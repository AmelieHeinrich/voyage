#pragma once

#include "sp_forward.h"
#include "sp_fxaa.h"
#include "sp_env_map.h"
#include "../entity/sp_scene.h"

struct sp_render_flow
{
    sp_forward forward;
	sp_env_map map;
    sp_fxaa fxaa;
};

void sp_render_flow_init(sp_render_flow* flow, HWND hwnd);
void sp_render_flow_free(sp_render_flow* flow);
void sp_render_flow_update(sp_render_flow* flow, sp_scene* scene);
void sp_render_flow_render(sp_render_flow* flow);
void sp_render_flow_resize(sp_render_flow* flow);