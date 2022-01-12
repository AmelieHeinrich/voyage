#pragma once

#include "../sp_common.h"
#include "../video/sp_mesh.h"

#include "sp_material.h"

#define SP_MAX_DRAWABLES 512

struct sp_render_update
{
    sp_model drawables[SP_MAX_DRAWABLES];
    i32 drawable_count;
};