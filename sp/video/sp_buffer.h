#pragma once

#include "../sp_common.h"
#include <d3d11.h>

enum class sp_buffer_usage
{
    vertex,
    index,
    uniform,
};

enum class sp_uniform_bind
{
    vertex,
    pixel,
    compute,
    geometry,
    hull,
    domain
};

struct sp_buffer
{
    ID3D11Buffer* buffer;
    i64 stride;
};

void sp_buffer_create(sp_buffer* buffer, i64 size, i64 stride, sp_buffer_usage usage);
void sp_buffer_free(sp_buffer* buffer);
void sp_buffer_set_data(sp_buffer* buffer, void* data);
void sp_buffer_bind_vb(sp_buffer* buffer);
void sp_buffer_bind_ib(sp_buffer* buffer);
void sp_buffer_bind_cb(sp_buffer* buffer, i32 binding, sp_uniform_bind bind);