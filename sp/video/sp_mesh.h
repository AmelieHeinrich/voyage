#pragma once

#include <vector>
#include "sp_buffer.h"

struct sp_vertex
{
    f32 px, py, pz;
    f32 nx, ny, nz;
    f32 tx, ty;
};

struct sp_mesh
{
    sp_buffer vertex_buffer;
    sp_buffer index_buffer;
    i32 vertex_count;
    i32 index_count;
};

struct sp_model
{
    std::vector<sp_mesh> meshes;
    char* path;
};

void sp_model_load(sp_model* mod, char* path);
void sp_model_free(sp_model* mod);