#pragma once

#include <Windows.h>
#include "sp_common.h"

void sp_game_init(HWND hwnd);
void sp_game_shutdown();
void sp_game_update();
void sp_game_resize(u32 width, u32 height);