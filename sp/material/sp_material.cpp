#include "sp_material.h"

#include "../video/sp_video.h"
#include "../sp_log.h"

#include <time.h>

// TODO(milo): instead of an id, make a unique material name
void sp_material_create(sp_material* mat, sp_material_info mat_info)
{
    mat->mat_info = mat_info;
    mat->name = mat_info.name;
	
    D3D11_RASTERIZER_DESC desc{};
    desc.CullMode = (D3D11_CULL_MODE)mat_info.cull_mode;
    desc.FillMode = (D3D11_FILL_MODE)mat_info.fill_mode;
    desc.FrontCounterClockwise = (BOOL)mat_info.ccw;
	
    HRESULT result = sp_video_data.device->CreateRasterizerState(&desc, &mat->rs_state);
    if (FAILED(result))
        sp_log_crit("Failed to create material rasterizer state");
	
    D3D11_DEPTH_STENCIL_DESC ds_desc{};
    ds_desc.DepthEnable = true;
    ds_desc.DepthFunc = (D3D11_COMPARISON_FUNC)mat_info.depth_op;
    ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	
    result = sp_video_data.device->CreateDepthStencilState(&ds_desc, &mat->ds_state);
    if (FAILED(result))
        sp_log_crit("Failed to create material depth stencil state");
}

void sp_material_free(sp_material* mat)
{
    safe_release(mat->ds_state);
    safe_release(mat->rs_state);
}

void sp_material_bind(sp_material* mat)
{
    sp_video_data.device_ctx->RSSetState(mat->rs_state);
    sp_video_data.device_ctx->OMSetDepthStencilState(mat->ds_state, 0);
}