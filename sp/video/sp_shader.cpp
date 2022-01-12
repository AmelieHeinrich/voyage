#include "sp_shader.h"
#include "../sp_file.h"
#include "../sp_log.h"
#include "../sp_common.h"
#include "sp_video.h"

#include <d3dcompiler.h>
#include <string>

ID3DBlob* sp_compile_blob(std::string source, const char* profile)
{
    ID3DBlob* shader_blob;
    ID3DBlob* error_blob;
    HRESULT status = D3DCompile(source.c_str(), source.size(), NULL, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile, 0, 0, &shader_blob, &error_blob);
    if (error_blob)
        sp_log_crit("Shader error (profile: %s) : %s", profile, (char*)error_blob->GetBufferPointer());
    return shader_blob;
}

void sp_shader_init(sp_shader* shader, const char* v, const char* p, const char* c, const char* g, const char* d, const char* h)
{
    ID3DBlob* vs = NULL;
    ID3DBlob* ps = NULL;
    ID3DBlob* cs = NULL;
    ID3DBlob* gs = NULL;
    ID3DBlob* ds = NULL;
    ID3DBlob* hs = NULL;

    if (v) vs = sp_compile_blob(sp_read_file(v), "vs_5_0");
    if (p) ps = sp_compile_blob(sp_read_file(p), "ps_5_0");
    if (c) cs = sp_compile_blob(sp_read_file(c), "cs_5_0");
    if (g) gs = sp_compile_blob(sp_read_file(g), "gs_5_0");
    if (d) ds = sp_compile_blob(sp_read_file(d), "ds_5_0");
    if (h) hs = sp_compile_blob(sp_read_file(h), "hs_5_0");

    if (vs) sp_video_data.device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &shader->vs);
    if (ps) sp_video_data.device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &shader->ps);
    if (cs) sp_video_data.device->CreateComputeShader(cs->GetBufferPointer(), cs->GetBufferSize(), NULL, &shader->cs);
    if (gs) sp_video_data.device->CreateGeometryShader(gs->GetBufferPointer(), gs->GetBufferSize(), NULL, &shader->gs);
    if (hs) sp_video_data.device->CreateHullShader(hs->GetBufferPointer(), hs->GetBufferSize(), NULL, &shader->hs);
    if (ds) sp_video_data.device->CreateDomainShader(ds->GetBufferPointer(), ds->GetBufferSize(), NULL, &shader->ds);

    safe_release(hs);
    safe_release(ds);
    safe_release(gs);
    safe_release(cs);
    safe_release(ps);
    safe_release(vs);
}

void sp_shader_free(sp_shader* shader)
{
    safe_release(shader->ds);
    safe_release(shader->hs);
    safe_release(shader->gs);
    safe_release(shader->cs);
    safe_release(shader->ps);
    safe_release(shader->vs);
}

void sp_shader_bind(sp_shader* shader)
{
    if (shader->vs) sp_video_data.device_ctx->VSSetShader(shader->vs, NULL, 0);
    if (shader->ps) sp_video_data.device_ctx->PSSetShader(shader->ps, NULL, 0);
    if (shader->cs) sp_video_data.device_ctx->CSSetShader(shader->cs, NULL, 0);
    if (shader->gs) sp_video_data.device_ctx->GSSetShader(shader->gs, NULL, 0);
    if (shader->hs) sp_video_data.device_ctx->HSSetShader(shader->hs, NULL, 0);
    if (shader->ds) sp_video_data.device_ctx->DSSetShader(shader->ds, NULL, 0);
}