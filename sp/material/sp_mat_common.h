#pragma once

#include "../sp_common.h"
#include "../video/sp_mesh.h"
#include "../video/sp_buffer.h"

#include "sp_material.h"
#include <glm/glm.hpp>

#define SP_MAX_DRAWABLES 512

struct sp_entity
{
    glm::mat4 transform;
    sp_model model;

    sp_buffer gpu_transform;
};

struct sp_render_update
{
    sp_entity drawables[SP_MAX_DRAWABLES];
    i32 drawable_count;
};