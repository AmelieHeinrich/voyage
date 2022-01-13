#pragma once

#include <string>
#include <glm/glm.hpp>
#include "../video/sp_mesh.h"
#include "../audio/sp_audio.h"

// megastructure kekw
struct sp_entity
{
    // Identifiers
    std::string name;
    i32 id;

    // Audio
    sp_audio_clip audio_clip;
    f32 volume;

    // Render component
    sp_model render_model;
    i32 material_index;

    // Transform
    sp_buffer gpu_transform;
    glm::mat4 transform;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};

void sp_entity_init(sp_entity* out, std::string name);
void sp_entity_free(sp_entity* entity);
void sp_entity_load_mesh(sp_entity* entity, std::string path);
void sp_entity_load_audio(sp_entity* entity, std::string path);
void sp_entity_set_material_index(sp_entity* entity, i32 material_index);
void sp_entity_set_position(sp_entity* entity, f32 x, f32 y, f32 z);
void sp_entity_set_scale(sp_entity* entity, f32 x, f32 y, f32 z);
void sp_entity_set_rotation(sp_entity* entity, f32 x, f32 y, f32 z);