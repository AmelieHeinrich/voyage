#pragma once

#include <d3d11.h>
#include "../sp_common.h"

enum class sp_fill_mode
{
    fill,
    line
};

enum class sp_cull_mode
{
    front,
    back,
    none
};

enum class sp_comp_op
{
    never = D3D11_COMPARISON_NEVER,
    less = D3D11_COMPARISON_LESS,
    equal = D3D11_COMPARISON_EQUAL,
    less_equal = D3D11_COMPARISON_LESS_EQUAL,
    greater = D3D11_COMPARISON_GREATER,
    not_equal = D3D11_COMPARISON_NOT_EQUAL,
    greater_equal = D3D11_COMPARISON_GREATER_EQUAL,
    always = D3D11_COMPARISON_ALWAYS
};

struct sp_material_info
{
    sp_fill_mode fill_mode;
    sp_cull_mode cull_mode;
    sp_comp_op depth_op;
    bool ccw;
};

struct sp_material
{
    ID3D11RasterizerState* rs_state;
    ID3D11DepthStencilState* ds_state;
    sp_material_info mat_info;
    u32 id;
};

void sp_material_create(sp_material* mat, sp_material_info mat_info);
void sp_material_free(sp_material* mat);
void sp_material_bind(sp_material* mat);