#include "sp_render_flow.h"

#include "../video/sp_video.h"

void sp_render_flow_init(sp_render_flow* flow)
{
    sp_forward_init(&flow->forward);

    sp_buffer_create(&flow->update.scene_buffer, sizeof(glm::mat4) * 2, 0, sp_buffer_usage::uniform);
}   

void sp_render_flow_free(sp_render_flow* flow)
{
    sp_forward_free(&flow->forward);
}

void sp_render_flow_update(sp_render_flow* flow)
{
    D3D11_VIEWPORT viewport{};
    viewport.Width = (FLOAT)sp_video_data.width;
    viewport.Height = (FLOAT)sp_video_data.height;
    sp_video_data.device_ctx->RSSetViewports(1, &viewport);

    sp_buffer_set_data(&flow->update.scene_buffer, &flow->update.camera);
    sp_forward_update(&flow->forward, flow->update);
}

void sp_render_flow_render(sp_render_flow* flow)
{
    sp_video_data.device_ctx->CopyResource(sp_video_data.swap_chain_buffer, flow->forward.rtv.texture);
}