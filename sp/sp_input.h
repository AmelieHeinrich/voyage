#pragma once

#include "sp_common.h"
#include "sp_key.h"

#include <glm/glm.hpp>

bool sp_key_pressed(u32 key);
bool sp_mouse_pressed(u32 btn);
f32 sp_get_mouse_x();
f32 sp_get_mouse_y();
glm::vec2 sp_get_mouse_pos();