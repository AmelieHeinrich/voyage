#include "sp_input.h"

#include <Windows.h>

bool sp_key_pressed(u32 key)
{
    return GetAsyncKeyState((int)key) & 0x8000;
}

bool sp_mouse_pressed(u32 btn)
{
    return GetAsyncKeyState((int)btn) & 0x8000;
}

glm::vec2 sp_get_mouse_pos()
{
    POINT p;
	GetCursorPos(&p);
	return glm::vec2(p.x, p.y);
}