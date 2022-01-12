#pragma once

#include "../sp_common.h"

#include <d3d11.h>
#include <dxgi.h>

struct sp_video
{
    HWND hwnd;
    u32 width, height;

    IDXGIAdapter* adapter;
    IDXGIFactory* factory;
    IDXGIDevice* dxgi;
    ID3D11Device* device;
    ID3D11DeviceContext* device_ctx;
    D3D_FEATURE_LEVEL level;

    IDXGISwapChain* swap_chain;
    ID3D11Texture2D* swap_chain_buffer;
    ID3D11RenderTargetView* swap_chain_rtv;
};

extern sp_video sp_video_data;

void sp_video_init(HWND hwnd);
void sp_video_begin();
void sp_video_resize(u32 width, u32 height);
void sp_video_present(bool vsync);
void sp_video_shutdown();
void sp_video_draw(i32 count, i32 start);
void sp_video_draw_indexed(i32 count, i32 start);