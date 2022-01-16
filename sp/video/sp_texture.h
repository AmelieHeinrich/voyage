#pragma once

#include <d3d11.h>
#include "../sp_common.h"
#include "sp_buffer.h"

#include <glm/glm.hpp>

enum sp_texture_bind 
{
    rtv = D3D11_BIND_RENDER_TARGET,
    dsv = D3D11_BIND_DEPTH_STENCIL,
    srv = D3D11_BIND_SHADER_RESOURCE,
    uav = D3D11_BIND_UNORDERED_ACCESS
};

enum class sp_texture_address
{
    wrap = D3D11_TEXTURE_ADDRESS_WRAP,
    mirror = D3D11_TEXTURE_ADDRESS_MIRROR,
    clamp = D3D11_TEXTURE_ADDRESS_CLAMP,
    border = D3D11_TEXTURE_ADDRESS_BORDER
};  

struct sp_sampler
{
    ID3D11SamplerState* ss;
    sp_texture_address address;
};

struct sp_texture
{
    ID3D11Texture2D* texture = nullptr;
    ID3D11Texture2D* depth_texture = nullptr;
    
    DXGI_FORMAT format;
    DXGI_FORMAT depth_format;
    i32 width, height;
	
    //
    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11DepthStencilView* dsv = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;
    ID3D11UnorderedAccessView* uav = nullptr;
};

void sp_sampler_init(sp_sampler* sampler, sp_texture_address address);
void sp_sampler_free(sp_sampler* sampler);
void sp_sampler_bind(sp_sampler* sampler, i32 binding, sp_uniform_bind bind);

void sp_texture_init(sp_texture* tex, i32 width, i32 height, DXGI_FORMAT format, u32 bind);
void sp_texture_init_cube(sp_texture* tex, i32 width, i32 height, DXGI_FORMAT format, u32 bind);
void sp_texture_load(sp_texture* tex, const char* path);
void sp_texture_load_float(sp_texture* tex, const char* path);
void sp_texture_free(sp_texture* tex);
void sp_texture_init_rtv(sp_texture* tex);
void sp_texture_init_dsv(sp_texture* tex, DXGI_FORMAT depth_format);
void sp_texture_init_srv(sp_texture* tex);
void sp_texture_init_srv_cube(sp_texture* tex);
void sp_texture_init_uav(sp_texture* tex);
void sp_texture_init_uav_cube(sp_texture* tex);

void sp_texture_bind_rtv(sp_texture* tex, glm::vec4 clear_color);
void sp_texture_bind_srv(sp_texture* tex, i32 binding, sp_uniform_bind bind);
void sp_texture_bind_uav(sp_texture* tex, i32 binding);

void sp_texture_reset_rtv();
void sp_texture_reset_uav(i32 binding);
void sp_texture_reset_srv(i32 binding, sp_uniform_bind bind);