#include <Windows.h>

#include "sp_common.h"
#include "sp_log.h"

#define GAME_WINDOW_CLASS_NAME "GameWindowClass"
#define GAME_WINDOW_TITLE "Game"
#define GAME_DEFAULT_WIDTH 1280
#define GAME_DEFAULT_HEIGHT 720

struct win32_state
{
    HINSTANCE hinstance;
    HWND hwnd;
    u32 width, height;
    bool running;
};

win32_state state;

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
}

void win32_update()
{
    while (state.running)
    {
        MSG msg;
        while (PeekMessageA(&msg, state.hwnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }
}

void win32_destroy()
{
    DestroyWindow(state.hwnd);
    sp_log_info("Terminated game");
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, i32 cmdShow)
{
    win32_create(hInstance);
    win32_update();
    win32_destroy();
}