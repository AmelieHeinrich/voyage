#include "sp_input.h"

#include <Windows.h>

sp_input_manager manager;

bool sp_key_pressed(u32 key)
{
    bool result = false;
    
    if (GetKeyboardState(manager.keys))
    {
        if (manager.keys[key] & 0x80)
            result = true;
        
        memcpy(manager.old_keys, manager.keys, sizeof(u8) * 256);
        
        return result;
    }
    
    return false;
}

bool sp_mouse_pressed(u32 btn)
{
    return GetAsyncKeyState((int)btn) & 0x8000;
}

f32 sp_get_mouse_x()
{
    return sp_get_mouse_pos().x;
}

f32 sp_get_mouse_y()
{
    return sp_get_mouse_pos().y;
}

glm::vec2 sp_get_mouse_pos()
{
    POINT p;
    GetCursorPos(&p);
    return glm::vec2(p.x, p.y);
}