#pragma once

#include <string>
#include <glm/glm.hpp>
#include "../video/sp_mesh.h"
#include "../audio/sp_audio.h"

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
	std::string material_name;
	
	// Light
	bool emits_light;
	glm::vec3 light_color;
	
    // Transform
    sp_buffer gpu_transform;
    glm::mat4 transform;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};

void sp_entity_init(sp_entity* out, const char* name);
void sp_entity_free(sp_entity* entity);
void sp_entity_load_mesh(sp_entity* entity, const char* path);
void sp_entity_load_audio(sp_entity* entity, const char* path);
void sp_entity_play_audio(sp_entity* entity);
void sp_entity_stop_audio(sp_entity* entity);
void sp_entity_loop_audio(sp_entity* entity, bool loop);
void sp_entity_set_material_name(sp_entity* entity, std::string material_name);
void sp_entity_set_position(sp_entity* entity, f32 x, f32 y, f32 z);
void sp_entity_set_scale(sp_entity* entity, f32 x, f32 y, f32 z);
void sp_entity_set_rotation(sp_entity* entity, f32 x, f32 y, f32 z);
void sp_entity_set_light_source(sp_entity* entity, bool light);
void sp_entity_set_light_color(sp_entity* entity, f32 x, f32 y, f32 z);
