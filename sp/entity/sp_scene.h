#pragma once

#include "sp_entity.h"
#include "../material/sp_material.h"
#include <unordered_map>

struct sp_scene
{
    std::unordered_map<u32, sp_entity> entities;
    std::unordered_map<u32, sp_material> materials;

    struct {
        glm::mat4 projection;
        glm::mat4 view;
    } scene_camera;
    sp_buffer camera_buffer;
};

void sp_scene_init(sp_scene* scene);
void sp_scene_free(sp_scene* scene);
void sp_scene_push_entity(sp_scene* scene, sp_entity entity);
void sp_scene_pop_entity(sp_scene* scene, sp_entity entity);
void sp_scene_push_material(sp_scene* scene, sp_material_info mat);
void sp_scene_pop_material(sp_scene* scene, i32 mat_index);
void sp_scene_update(sp_scene* scene);