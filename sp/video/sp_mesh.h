#pragma once

#include <vector>
#include <string>
#include "sp_buffer.h"
#include "sp_texture.h"

struct sp_vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct sp_mesh
{
    sp_buffer vertex_buffer;
    sp_buffer index_buffer;
    i32 vertex_count;
    i32 index_count;

    sp_texture albedo_texture;
    sp_texture metallic_roughness_texture;
    sp_texture normal_texture;
};

struct sp_model
{
    std::vector<sp_mesh> meshes;
    std::string directory;
};

void sp_model_load(sp_model* mod, const std::string& path);
void sp_model_free(sp_model* mod);