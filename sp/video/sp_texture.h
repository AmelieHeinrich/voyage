#pragma once

#include <d3d11.h>
#include "../sp_common.h"
#include "sp_buffer.h"

#include <glm/glm.hpp>

enum class sp_texture_bind
{
    rtv = D3D11_BIND_RENDER_TARGET,
    dsv = D3D11_BIND_DEPTH_STENCIL,
    srv = D3D11_BIND_SHADER_RESOURCE,
    uav = D3D11_BIND_UNORDERED_ACCESS
};

struct sp_texture
{
    ID3D11Texture2D* texture;
    ID3D11Texture2D* depth_texture;
    
    DXGI_FORMAT format;
    DXGI_FORMAT depth_format;
    i32 width, height;
    sp_texture_bind bind;

    //
    ID3D11RenderTargetView* rtv;
    ID3D11DepthStencilView* dsv;
    ID3D11ShaderResourceView* srv;
    ID3D11UnorderedAccessView* uav;
};

void sp_texture_init(sp_texture* tex, i32 width, i32 height, DXGI_FORMAT format, sp_texture_bind bind);
void sp_texture_free(sp_texture* tex);
void sp_texture_init_rtv(sp_texture* tex);
void sp_texture_init_dsv(sp_texture* tex, DXGI_FORMAT depth_format);
void sp_texture_init_srv(sp_texture* tex);
void sp_texture_init_uav(sp_texture* tex);

void sp_texture_bind_rtv(sp_texture* tex, glm::vec4 clear_color);
void sp_texture_bind_srv(sp_texture* tex, i32 binding, sp_uniform_bind bind);
void sp_texture_bind_uav(sp_texture* tex, i32 binding);