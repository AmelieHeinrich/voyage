#include <Windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include "sp_common.h"
#include "sp_log.h"
#include "video/sp_video.h"
#include "video/sp_shader.h"

#define GAME_WINDOW_CLASS_NAME "GameWindowClass"
#define GAME_WINDOW_TITLE "Game"
#define GAME_DEFAULT_WIDTH 1280
#define GAME_DEFAULT_HEIGHT 720

struct game_state
{
    HINSTANCE hinstance;

    HWND hwnd;
    u32 width, height;
    bool running;

    //
    sp_shader forward_shader;
};

game_state state;

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        state.running = 1;
        break;
    }

    case WM_DESTROY:
    {
        state.running = 0;
        break;
    }

    case WM_SIZE:
    {
        if (sp_video_data.swap_chain)
            sp_video_resize(LOWORD(lparam), HIWORD(lparam));
    }

    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

void win32_create(HINSTANCE hInstance)
{
    state.width = GAME_DEFAULT_WIDTH;
    state.height = GAME_DEFAULT_HEIGHT;
    state.hinstance = hInstance;

    WNDCLASSA window_class{};
    window_class.lpfnWndProc = wnd_proc;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = GAME_WINDOW_CLASS_NAME;
    RegisterClassA(&window_class);

    state.hwnd = CreateWindowA(window_class.lpszClassName,
            GAME_WINDOW_TITLE,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            state.width, state.height,
            NULL, NULL, hInstance,
            NULL);

    sp_log_info("Initialised game");

    ShowWindow(state.hwnd, SW_SHOWDEFAULT);

    sp_video_init(state.hwnd);
    sp_shader_init(&state.forward_shader, "data/shaders/forward/forward_vs.hlsl", "data/shaders/forward/forward_ps.hlsl");
}

void win32_update()
{
    MSG msg;
    while (PeekMessageA(&msg, state.hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void win32_destroy()
{
    sp_shader_free(&state.forward_shader);
    sp_video_shutdown();
    DestroyWindow(state.hwnd);
    sp_log_info("Terminated game");
}

int main()
{
    win32_create(GetModuleHandle(NULL));
    while (state.running)
    {
        win32_update();
        sp_video_begin();
        sp_video_present(1);
    }
    win32_destroy();
}