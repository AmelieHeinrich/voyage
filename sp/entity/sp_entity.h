#pragma once

#include <string>
#include <glm/glm.hpp>
#include "../video/sp_mesh.h"
#include "../audio/sp_audio.h"

struct sp_entity;
void sp_entity_init(sp_entity* out, const char* name);
void sp_entity_free(sp_entity* entity);
void sp_entity_load_mesh(sp_entity* entity, const char* path);
void sp_entity_load_audio(sp_entity* entity, const char* path);
void sp_entity_set_material_index(sp_entity* entity, i32 material_index);
void sp_entity_set_position(sp_entity* entity, f32 x, f32 y, f32 z);
void sp_entity_set_scale(sp_entity* entity, f32 x, f32 y, f32 z);
void sp_entity_set_rotation(sp_entity* entity, f32 x, f32 y, f32 z);

// megastructure kekw
struct sp_entity
{
    // Identifiers
    char* name;
    i32 id;
    bool scripted;

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

    // lua registered functions (gosh this is so trash)
    void load_mesh(const char* path) { sp_entity_load_mesh(this, path); }
    void load_audio(const char* path) { sp_entity_load_audio(this, path); }
    void set_material_index(i32 index) { sp_entity_set_material_index(this, index); }
    void set_position(f32 x, f32 y, f32 z) { sp_entity_set_position(this, x, y, z); }
    void set_scale(f32 x, f32 y, f32 z) { sp_entity_set_scale(this, x, y, z); }
    void set_rotation(f32 x, f32 y, f32 z) { sp_entity_set_rotation(this, x, y, z); }
};