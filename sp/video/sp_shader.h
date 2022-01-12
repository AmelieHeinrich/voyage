#pragma once

#include <d3d11.h>

struct sp_shader
{
    ID3D11VertexShader*   vs = NULL;
    ID3D11PixelShader*    ps = NULL;
    ID3D11ComputeShader*  cs = NULL;
    ID3D11GeometryShader* gs = NULL;
    ID3D11DomainShader*   ds = NULL;
    ID3D11HullShader*     hs = NULL;
};

void sp_shader_init(sp_shader* shader, const char* v = NULL, const char* p = NULL, const char* c = NULL, const char* g = NULL, const char* d = NULL, const char* h = NULL);
void sp_shader_free(sp_shader* shader);
void sp_shader_bind(sp_shader* shader);