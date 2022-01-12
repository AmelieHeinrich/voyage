#pragma once

#include "../sp_common.h"
#include "../video/sp_buffer.h"

#include "sp_material.h"

#define SP_MAX_DRAWABLES 512

struct sp_drawable
{
    sp_material material;
    sp_buffer vertex_buffer;
    i32 vertex_count;
};

struct sp_render_update
{
    sp_drawable drawables[SP_MAX_DRAWABLES];
    i32 drawable_count;
};