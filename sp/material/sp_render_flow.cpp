#include "sp_render_flow.h"

#include "../video/sp_video.h"
#include "sp_gui.h"
#include "../sp_platform.h"
#include "../sp_game.h"
#include "../debug/sp_cvar.h"

sp_cvar* skybox;

void sp_render_flow_init(sp_render_flow* flow, HWND hwnd)
{
	skybox = cvar_registry.get("mat_toggle_env");
	
    sp_forward_init(&flow->forward);
	sp_env_map_init(&flow->map, "data/env/Street_High.hdr");
    sp_fxaa_init(&flow->fxaa);
	
	if (platform_data.enable_gui)
		sp_gui_init(hwnd);
}   

void sp_render_flow_free(sp_render_flow* flow)
{
	if (platform_data.enable_gui)
		sp_gui_free();
	
    sp_fxaa_free(&flow->fxaa);
	sp_env_map_free(&flow->map);
    sp_forward_free(&flow->forward);
}

void sp_render_flow_update(sp_render_flow* flow, sp_scene* scene)
{
    D3D11_VIEWPORT viewport{};
    viewport.Width = (FLOAT)sp_video_data.width;
    viewport.Height = (FLOAT)sp_video_data.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    sp_video_data.device_ctx->RSSetViewports(1, &viewport);
	
    sp_forward_update(&flow->forward, scene, &flow->map);
	if (skybox->as.i)
		sp_env_map_update(&flow->map, scene);
    sp_fxaa_update(&flow->fxaa, &flow->forward.rtv);
}

void sp_render_flow_render(sp_render_flow* flow)
{
    sp_video_data.device_ctx->CopyResource(sp_video_data.swap_chain_buffer, flow->fxaa.fxaa_texture.texture);
}

void sp_render_flow_resize(sp_render_flow* flow)
{
    sp_fxaa_resize(&flow->fxaa);
    sp_forward_resize(&flow->forward);
}