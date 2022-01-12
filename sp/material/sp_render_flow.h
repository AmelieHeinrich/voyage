#pragma once

#include "sp_forward.h"

struct sp_render_flow
{
    sp_render_update update;
    sp_forward forward;
};

void sp_render_flow_init(sp_render_flow* flow);
void sp_render_flow_free(sp_render_flow* flow);
void sp_render_flow_update(sp_render_flow* flow);