#include "sp_buffer.h"

#include "sp_video.h"
#include "../sp_log.h"

D3D11_BIND_FLAG sp_usage_to_d3d11(sp_buffer_usage usage)
{
    switch (usage)
    {
    case sp_buffer_usage::vertex:
        return D3D11_BIND_VERTEX_BUFFER;
    case sp_buffer_usage::index:
        return D3D11_BIND_INDEX_BUFFER;
    case sp_buffer_usage::uniform:
        return D3D11_BIND_CONSTANT_BUFFER;
    }

    return D3D11_BIND_VERTEX_BUFFER;
}

void sp_buffer_create(sp_buffer* buffer, i64 size, i64 stride, sp_buffer_usage usage)
{
    buffer->stride = stride;

    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = (UINT)size;
    desc.BindFlags = sp_usage_to_d3d11(usage);
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0; 

    HRESULT res = sp_video_data.device->CreateBuffer(&desc, NULL, &buffer->buffer);
    if (FAILED(res))
        sp_log_crit("Failed to create buffer with size %d", size);
}

void sp_buffer_free(sp_buffer* buffer)
{
    safe_release(buffer->buffer);
}

void sp_buffer_set_data(sp_buffer* buffer, void* data)
{
    sp_video_data.device_ctx->UpdateSubresource(buffer->buffer, NULL, NULL, data, NULL, NULL);
}

void sp_buffer_bind_vb(sp_buffer* buffer)
{
    UINT stride = (UINT)buffer->stride;
    UINT offset = 0;
    sp_video_data.device_ctx->IASetVertexBuffers(0, 1, &buffer->buffer, &stride, &offset);
}

void sp_buffer_bind_ib(sp_buffer* buffer)
{
    sp_video_data.device_ctx->IASetIndexBuffer(buffer->buffer, DXGI_FORMAT_R32_UINT, 0);
}

void sp_buffer_bind_cb(sp_buffer* buffer, i32 binding, sp_uniform_bind bind)
{   
    switch (bind)
    {
    case sp_uniform_bind::vertex:
        sp_video_data.device_ctx->VSSetConstantBuffers(binding, 1, &buffer->buffer);
        break;
    case sp_uniform_bind::pixel:
        sp_video_data.device_ctx->PSSetConstantBuffers(binding, 1, &buffer->buffer);
        break;
    case sp_uniform_bind::compute:
        sp_video_data.device_ctx->CSSetConstantBuffers(binding, 1, &buffer->buffer);
        break;
    case sp_uniform_bind::geometry:
        sp_video_data.device_ctx->GSSetConstantBuffers(binding, 1, &buffer->buffer);
        break;
    case sp_uniform_bind::hull:
        sp_video_data.device_ctx->HSSetConstantBuffers(binding, 1, &buffer->buffer);
        break;
    case sp_uniform_bind::domain:
        sp_video_data.device_ctx->DSSetConstantBuffers(binding, 1, &buffer->buffer);
        break;
    }
}