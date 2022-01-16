#include <Windows.h>

#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui.h>

#include "sp_common.h"
#include "sp_log.h"
#include "sp_game.h"
#include "sp_platform.h"

// TODO(milo): load config from file?
#define GAME_WINDOW_CLASS_NAME "GameWindowClass"
#define GAME_WINDOW_TITLE "Voyage"
#define GAME_DEFAULT_WIDTH 1280
#define GAME_DEFAULT_HEIGHT 720

struct game_state
{
    HINSTANCE hinstance;
    HWND hwnd;
};

game_state state;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (platform_data.enable_gui)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			return 1;
	}
	
    switch (msg)
    {
		case WM_CREATE:
		{
			platform_data.running = 1;
			break;
		}
		
		case WM_DESTROY:
		{
			platform_data.running = 0;
			break;
		}
		
		case WM_SIZE:
		{
			platform_data.width = (i32)LOWORD(lparam);
			platform_data.height = (i32)HIWORD(lparam);
			sp_game_resize(LOWORD(lparam), HIWORD(lparam));
			break;
		}
		
		case WM_DPICHANGED: {
			// NOTE(milo): lol fuck imgui viewports ngl
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
			{
				const RECT* suggested_rect = (RECT*)lparam;
				::SetWindowPos(hwnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			break;
		}
		
		default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
    }
	
    return 0;
}

void win32_create(HINSTANCE hInstance)
{
    platform_data.width = GAME_DEFAULT_WIDTH;
    platform_data.height = GAME_DEFAULT_HEIGHT;
	platform_data.enable_gui = true;
    state.hinstance = hInstance;
	
    WNDCLASSA window_class{};
    window_class.lpfnWndProc = wnd_proc;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = GAME_WINDOW_CLASS_NAME;
	window_class.hbrBackground = (HBRUSH)COLOR_WINDOW;
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&window_class);
	
    state.hwnd = CreateWindowA(window_class.lpszClassName,
							   GAME_WINDOW_TITLE,
							   WS_OVERLAPPEDWINDOW,
							   CW_USEDEFAULT, CW_USEDEFAULT,
							   platform_data.width, platform_data.height,
							   NULL, NULL, hInstance,
							   NULL);
	
    ShowWindow(state.hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(state.hwnd);
    sp_game_init(state.hwnd);
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
    sp_game_shutdown();
    DestroyWindow(state.hwnd);
}

int main()
{
    win32_create(GetModuleHandle(NULL));
    while (platform_data.running)
    {
        win32_update();
        sp_game_update();   
    }
    win32_destroy();
}