#pragma once

#include "../video/sp_shader.h"
#include "sp_mat_common.h"

struct sp_forward
{
    sp_shader forward_shader;
};

void sp_forward_init(sp_forward* forward);
void sp_forward_free(sp_forward* forward);
void sp_forward_update(sp_forward* forward, sp_render_update update);