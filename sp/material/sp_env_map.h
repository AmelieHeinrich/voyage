#pragma once

#include "../video/sp_shader.h"
#include "../video/sp_texture.h"
#include "../video/sp_buffer.h"
#include "../entity/sp_scene.h"

#include "sp_material.h"

#include <glm/glm.hpp>

struct sp_env_map
{
	// Path of the environment map
	char* path;
	
	// Equirectangular to cubemap shader
	sp_shader equi_shader;
	// Irradiance compute shader
	sp_shader irradiance_shader;
	// Prefilter compute shader
	sp_shader prefilter_shader;
	// BRDF compute shader
	sp_shader brdf_shader;
	// Shader that draws the actual environment map
	sp_shader draw_shader;
	// Cull none
	sp_material cube_material;
	
	// HDR texture
	sp_texture equi_map;
	// Cubemap texture
	sp_texture cube_map;
	// Irradiance map
	sp_texture irradiance_map;
	// Prefilter map
	sp_texture prefilter_map;
	// BRDF map
	sp_texture brdf_map;
	// Cubemap sampler
	sp_sampler cube_map_sampler;
	// Cube vertices
	sp_buffer cube_map_buffer;
	// Uniform buffer
	sp_buffer cube_upd_buffer;
};

void sp_env_map_init(sp_env_map* map, const char* path);
void sp_env_map_free(sp_env_map* map);
void sp_env_map_update(sp_env_map* map, sp_scene* scene);
void sp_env_map_restart(sp_env_map* map, char* path); 
