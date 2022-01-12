#include "sp_texture.h"

#include "sp_video.h"
#include "../sp_log.h"

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

void sp_sampler_init(sp_sampler* sampler, sp_texture_address address)
{
    sampler->address = address;

    D3D11_SAMPLER_DESC desc{};
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)address;
    desc.AddressV = desc.AddressU;
    desc.AddressW = desc.AddressV;
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 1;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT res = sp_video_data.device->CreateSamplerState(&desc, &sampler->ss);
    if (FAILED(res))
        sp_log_crit("Failed to create sampler state");
}

void sp_sampler_free(sp_sampler* sampler)
{
    safe_release(sampler->ss);
}

void sp_sampler_bind(sp_sampler* sampler, i32 binding, sp_uniform_bind bind)
{
    switch (bind)
    {
    case sp_uniform_bind::vertex:
        sp_video_data.device_ctx->VSSetSamplers(binding, 1, &sampler->ss);
        break;
    case sp_uniform_bind::pixel:
        sp_video_data.device_ctx->PSSetSamplers(binding, 1, &sampler->ss);
        break;
    case sp_uniform_bind::compute:
        sp_video_data.device_ctx->CSSetSamplers(binding, 1, &sampler->ss);
        break;
    case sp_uniform_bind::geometry:
        sp_video_data.device_ctx->GSSetSamplers(binding, 1, &sampler->ss);
        break;
    case sp_uniform_bind::hull:
        sp_video_data.device_ctx->HSSetSamplers(binding, 1, &sampler->ss);
        break;
    case sp_uniform_bind::domain:
        sp_video_data.device_ctx->DSSetSamplers(binding, 1, &sampler->ss);
        break;
    }
}

void sp_texture_init(sp_texture* tex, i32 width, i32 height, DXGI_FORMAT format, sp_texture_bind bind)
{
    tex->width = width;
    tex->height = height;
    tex->bind = bind;
    tex->format = format;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.Format = format;
    desc.ArraySize = 1;
    desc.BindFlags = (D3D11_BIND_FLAG)bind;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.SampleDesc.Count = 1;
    desc.MipLevels = 1;

    HRESULT res = sp_video_data.device->CreateTexture2D(&desc, NULL, &tex->texture);
    if (FAILED(res))
        sp_log_crit("Failed to create D3D11 image");
}

void sp_texture_load(sp_texture* tex, const char* path)
{
    i32 channels = 0;
    stbi_set_flip_vertically_on_load(1);
    u8* buf = stbi_load(path, &tex->width, &tex->height, &channels, STBI_rgb_alpha);
    if (!buf)
        sp_log_crit("Failed to load texture file with path %s", path);

    tex->bind = sp_texture_bind::srv;
    tex->format = DXGI_FORMAT_R8G8B8A8_UNORM;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = tex->width;
    desc.Height = tex->height;
    desc.Format = tex->format;
    desc.ArraySize = 1;
    desc.BindFlags = (D3D11_BIND_FLAG)tex->bind;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.SampleDesc.Count = 1;
    desc.MipLevels = 1;

    D3D11_SUBRESOURCE_DATA subresource{};
    subresource.pSysMem = buf;
    subresource.SysMemPitch = 4 * tex->width;
    subresource.SysMemSlicePitch = 4 * tex->width * tex->height;

    HRESULT res = sp_video_data.device->CreateTexture2D(&desc, &subresource, &tex->texture);
    if (FAILED(res))
        sp_log_crit("Failed to create D3D11 image");

    stbi_image_free(buf);
}

void sp_texture_free(sp_texture* tex)
{
    safe_release(tex->uav); 
    safe_release(tex->srv);
    safe_release(tex->dsv);
    safe_release(tex->rtv);
    safe_release(tex->depth_texture);
    safe_release(tex->texture);
}

void sp_texture_init_rtv(sp_texture* tex)
{
    HRESULT res = sp_video_data.device->CreateRenderTargetView(tex->texture, NULL, &tex->rtv);
    if (FAILED(res))
        sp_log_crit("Failed to create render target view");
}

void sp_texture_init_dsv(sp_texture* tex, DXGI_FORMAT depth_format)
{
    tex->depth_format = depth_format;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = tex->width;
    desc.Height = tex->height;
    desc.Format = depth_format;
    desc.ArraySize = 1;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.SampleDesc.Count = 1;
    desc.MipLevels = 1;

    HRESULT res = sp_video_data.device->CreateTexture2D(&desc, NULL, &tex->depth_texture);
    if (FAILED(res))
        sp_log_crit("Failed to create D3D11 image");
        
    res = sp_video_data.device->CreateDepthStencilView(tex->depth_texture, NULL, &tex->dsv);
    if (FAILED(res))
        sp_log_crit("Failed to create depth stencil view");
}

void sp_texture_init_srv(sp_texture* tex)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = tex->format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

    HRESULT res = sp_video_data.device->CreateShaderResourceView(tex->texture, &shaderResourceViewDesc, &tex->srv);
    if (FAILED(res))
        sp_log_crit("Failed to create shader resource view");
}

void sp_texture_init_uav(sp_texture* tex)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC desc{};
    desc.Format = tex->format;
    desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;

    HRESULT res = sp_video_data.device->CreateUnorderedAccessView(tex->texture, &desc, &tex->uav);
    if (FAILED(res))
        sp_log_crit("Failed to create unordered access view");
}

void sp_texture_bind_rtv(sp_texture* tex, glm::vec4 clear_color)
{
    sp_video_data.device_ctx->OMSetRenderTargets(1, &tex->rtv, tex->dsv);
    sp_video_data.device_ctx->ClearRenderTargetView(tex->rtv, glm::value_ptr(clear_color));
    if (tex->dsv)
        sp_video_data.device_ctx->ClearDepthStencilView(tex->dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void sp_texture_bind_srv(sp_texture* tex, i32 binding, sp_uniform_bind bind)
{
    switch (bind)
    {
    case sp_uniform_bind::vertex:
        sp_video_data.device_ctx->VSSetShaderResources(binding, 1, &tex->srv);
        break;
    case sp_uniform_bind::pixel:
        sp_video_data.device_ctx->PSSetShaderResources(binding, 1, &tex->srv);
        break;
    case sp_uniform_bind::compute:
        sp_video_data.device_ctx->CSSetShaderResources(binding, 1, &tex->srv);
        break;
    case sp_uniform_bind::geometry:
        sp_video_data.device_ctx->GSSetShaderResources(binding, 1, &tex->srv);
        break;
    case sp_uniform_bind::hull:
        sp_video_data.device_ctx->HSSetShaderResources(binding, 1, &tex->srv);
        break;
    case sp_uniform_bind::domain:
        sp_video_data.device_ctx->DSSetShaderResources(binding, 1, &tex->srv);
        break;
    }
}

void sp_texture_bind_uav(sp_texture* tex, i32 binding)
{
    sp_video_data.device_ctx->CSSetUnorderedAccessViews(binding, 1, &tex->uav, NULL);
}
