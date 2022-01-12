#pragma once

#include "../sp_common.h"

#include <glm/glm.hpp>

struct sp_debug_camera
{
    f32 sensitivity;
    f32 yaw;
    f32 pitch;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldup;
    glm::vec2 mouse_pos;

    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 view_projection;
    
    f32 width;
    f32 height;

    f32 acceleration;
    f32 friction;
    glm::vec3 velocity;
    f32 max_velocity;
};

void sp_debug_camera_init(sp_debug_camera* cam);
void sp_debug_camera_update(sp_debug_camera* cam, f32 dt);
void sp_debug_camera_input(sp_debug_camera* cam, f32 dt);