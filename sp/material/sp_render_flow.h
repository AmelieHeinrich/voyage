#pragma once

#include "sp_forward.h"
#include "sp_fxaa.h"
#include "../entity/sp_scene.h"

struct sp_render_flow
{
    sp_forward forward;
    sp_fxaa fxaa;
};

void sp_render_flow_init(sp_render_flow* flow);
void sp_render_flow_free(sp_render_flow* flow);
void sp_render_flow_update(sp_render_flow* flow, sp_scene* scene);
void sp_render_flow_render(sp_render_flow* flow);
void sp_render_flow_resize(sp_render_flow* flow);