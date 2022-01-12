#pragma once

#include <d3d11.h>

struct sp_shader
{
    ID3D11VertexShader* vs;
    ID3D11PixelShader* ps;
    ID3D11ComputeShader* cs;
    ID3D11GeometryShader* gs;
    ID3D11DomainShader* ds;
    ID3D11HullShader* hs;

    ID3D11InputLayout* input_layout;
};

void sp_shader_init(sp_shader* shader, const char* v = NULL, const char* p = NULL, const char* c = NULL, const char* g = NULL, const char* d = NULL, const char* h = NULL);
void sp_shader_free(sp_shader* shader);
void sp_shader_bind(sp_shader* shader);