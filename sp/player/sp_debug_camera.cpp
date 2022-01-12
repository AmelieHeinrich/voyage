#include "sp_debug_camera.h"

#include "../video/sp_video.h"
#include "../sp_input.h"

#include <glm/gtc/matrix_transform.hpp>

void sp_debug_camera_refresh(sp_debug_camera* cam)
{
    glm::vec3 front(0.0f);
    front.x = (f32)(cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch)));
    front.y = (f32)(sin(glm::radians(cam->pitch)));
    front.z = (f32)(sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch)));
    cam->front = glm::normalize(front);

    cam->right = glm::cross(cam->front, cam->worldup);
    cam->up = glm::cross(cam->right, cam->front);
}

void sp_debug_camera_init(sp_debug_camera* cam)
{
    cam->sensitivity = 5.0f;
    cam->up.y = 1.0f;
    cam->front.y = -1.0f;
    cam->worldup.y = 1.0f;
    cam->yaw = -90.0f;
    cam->pitch = 0.0f;
    cam->friction = 10.0f;
    cam->acceleration = 20.0f;
    cam->max_velocity = 15.0f;
    cam->width = (f32)sp_video_data.width;
    cam->height = (f32)sp_video_data.height;

    sp_debug_camera_refresh(cam);
}

void sp_debug_camera_update(sp_debug_camera* cam, f32 dt)
{
    cam->mouse_pos = sp_get_mouse_pos();

    cam->view = glm::lookAt(cam->position, cam->position + cam->front, cam->up);
    cam->projection = glm::perspective(glm::radians(90.0f), (f32)(cam->width / cam->height), 0.0001f, 100000.0f);
}

void sp_debug_camera_input(sp_debug_camera* cam, f32 dt)
{
    f32 speed_multiplier = cam->acceleration * dt;
    if (sp_key_pressed(SP_KEY_Z))
        cam->velocity += cam->front * speed_multiplier;
    if (sp_key_pressed(SP_KEY_S))
        cam->velocity -= cam->front * speed_multiplier;
    if (sp_key_pressed(SP_KEY_D))
        cam->velocity += cam->right * speed_multiplier;
    if (sp_key_pressed(SP_KEY_Q))
        cam->velocity -= cam->right * speed_multiplier;
    
    f32 friction_multiplier = 1.0f / (1.0f + (cam->friction * dt));
    cam->velocity *= friction_multiplier;
    
    f32 length = glm::length(cam->velocity);
    if (length > cam->max_velocity)
        cam->velocity *= (cam->max_velocity * glm::normalize(cam->velocity));
    
    cam->position += cam->velocity * dt;

    glm::vec2 mouse = sp_get_mouse_pos();
    f32 dx = (mouse.x - cam->mouse_pos.x) * (cam->sensitivity * dt);
    f32 dy = (mouse.y - cam->mouse_pos.y) * (cam->sensitivity * dt);

    if (sp_mouse_pressed(SP_MOUSE_LEFT))
    {
        cam->yaw += dx;
        cam->pitch -= dy; // Reverse Y axis
    }

    sp_debug_camera_refresh(cam);
}