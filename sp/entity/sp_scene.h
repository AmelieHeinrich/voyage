#pragma once

#include "sp_entity.h"
#include "../material/sp_material.h"
#include <unordered_map>
#include <string>

// TODO(milo): load scene from file

struct sp_scene
{
    std::unordered_map<u32, sp_entity> entities;
    std::unordered_map<std::string, sp_material> materials;
	
    struct {
        glm::mat4 projection;
        glm::mat4 view;
		glm::vec4 camera_position;
    } scene_camera;
	
	struct {
		// NOTE(milo): 0=pbr, 1=albedo, 2=metalroughness, 3=normals, 4=diffuse, 5=specular, 6=none
		i32 render_mode;
		// NOTE(milo): 0=off, 1=on
		bool force_wireframe;
		// NOTE(milo): vec2 for 16 byte alignment
		glm::vec2 padding0;
	} scene_render;
	
	sp_buffer scene_render_buffer;
    sp_buffer camera_buffer;
	sp_material wireframe_material;
};

void sp_scene_init(sp_scene* scene);
void sp_scene_free(sp_scene* scene);
void sp_scene_push_entity(sp_scene* scene, sp_entity entity);
void sp_scene_pop_entity(sp_scene* scene, sp_entity entity);
void sp_scene_push_material(sp_scene* scene, sp_material_info mat);
void sp_scene_pop_material(sp_scene* scene, std::string material_name);

void sp_scene_on_init(sp_scene* scene);
void sp_scene_on_update(sp_scene* scene);
void sp_scene_on_free(sp_scene* scene);