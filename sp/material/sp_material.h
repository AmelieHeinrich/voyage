#pragma once

#include <d3d11.h>

enum sp_fill_mode
{
    fill,
    line
};

enum sp_cull_mode
{
    front,
    back,
    none
};

struct sp_material_info
{
    sp_fill_mode fill_mode;
    sp_cull_mode cull_mode;
    bool ccw;
};

struct sp_material
{
    ID3D11RasterizerState* rs_state;
    sp_material_info mat_info;
};

void sp_material_create(sp_material* mat, sp_material_info mat_info);
void sp_material_free(sp_material* mat);
void sp_material_bind(sp_material* mat);