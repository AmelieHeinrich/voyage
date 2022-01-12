#include "sp_texture.h"

#include "sp_video.h"
#include "../sp_log.h"

#include <glm/gtc/type_ptr.hpp>

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

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = depth_format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

    res = sp_video_data.device->CreateDepthStencilView(tex->depth_texture, &depthStencilViewDesc, &tex->dsv);
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
        sp_video_data.device_ctx->ClearDepthStencilView(tex->dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
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
