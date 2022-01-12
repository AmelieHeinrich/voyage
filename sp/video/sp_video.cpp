#include "sp_video.h"
#include "../sp_log.h"

const D3D_DRIVER_TYPE driver_types[] =
{
	D3D_DRIVER_TYPE_HARDWARE,
	D3D_DRIVER_TYPE_WARP,
	D3D_DRIVER_TYPE_REFERENCE,
};

sp_video sp_video_data;

void sp_video_init(HWND hwnd)
{
    sp_video_data.hwnd = hwnd;
    
    D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0};
    HRESULT result = 0;
    for (i32 i = 0; i < ARRAYSIZE(driver_types);)
    {
        result = D3D11CreateDevice(NULL, driver_types[i], NULL, D3D11_CREATE_DEVICE_DEBUG, levels, 1, D3D11_SDK_VERSION, &sp_video_data.device, &sp_video_data.level, &sp_video_data.device_ctx);

        if (SUCCEEDED(result))
            break;
        ++i;
    }

    if (FAILED(result))
        sp_log_crit("Failed to initialise D3D11 device.");
    
    sp_video_data.device->QueryInterface(IID_PPV_ARGS(&sp_video_data.dxgi));
    sp_video_data.dxgi->GetParent(IID_PPV_ARGS(&sp_video_data.adapter));
    sp_video_data.adapter->GetParent(IID_PPV_ARGS(&sp_video_data.factory));

    RECT rect;
    GetClientRect(hwnd, &rect);
    sp_video_data.width = rect.right - rect.left;
    sp_video_data.height = rect.bottom - rect.top;

    sp_video_resize(sp_video_data.width, sp_video_data.height);
}

void sp_video_begin()
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)sp_video_data.width;
    viewport.Height = (FLOAT)sp_video_data.height;

    sp_video_data.device_ctx->RSSetViewports(1, &viewport);
    sp_video_data.device_ctx->OMSetRenderTargets(1, &sp_video_data.swap_chain_rtv, NULL);
}

void sp_video_resize(u32 width, u32 height)
{
    sp_video_data.width = width;
    sp_video_data.height = height;

    if (!sp_video_data.swap_chain)
    {
        DXGI_SWAP_CHAIN_DESC swap_desc{};
		swap_desc.BufferDesc.Width = width;
		swap_desc.BufferDesc.Height = height;
		swap_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap_desc.SampleDesc.Count = 1;		
		swap_desc.SampleDesc.Quality = 0;	
		swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_desc.BufferCount = 1;
		swap_desc.OutputWindow = sp_video_data.hwnd;
		swap_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swap_desc.Windowed = TRUE;

        HRESULT result = sp_video_data.factory->CreateSwapChain(sp_video_data.device, &swap_desc, &sp_video_data.swap_chain);
        if (FAILED(result))
            sp_log_crit("Failed to create swap chain");
    }

    safe_release(sp_video_data.swap_chain_rtv);
    safe_release(sp_video_data.swap_chain_buffer);

    sp_video_data.swap_chain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    sp_video_data.swap_chain->GetBuffer(0, IID_PPV_ARGS(&sp_video_data.swap_chain_buffer));
    sp_video_data.device->CreateRenderTargetView(sp_video_data.swap_chain_buffer, NULL, &sp_video_data.swap_chain_rtv);
}

void sp_video_present(bool vsync)
{
    sp_video_data.swap_chain->Present(vsync, 0);
}

void sp_video_shutdown()
{
    safe_release(sp_video_data.swap_chain_rtv);
    safe_release(sp_video_data.swap_chain_buffer);
    safe_release(sp_video_data.swap_chain);
    safe_release(sp_video_data.factory);
    safe_release(sp_video_data.adapter);
    safe_release(sp_video_data.dxgi);
    safe_release(sp_video_data.device);
}

void sp_video_draw(i32 count, i32 start)
{
    sp_video_data.device_ctx->Draw(count, start);
}

void sp_video_draw_indexed(i32 count, i32 start)
{
    sp_video_data.device_ctx->DrawIndexed(count, start, 0);
}