#pragma once

#include "sp_common.h"

struct sp_platform
{
	bool enable_gui;
	bool fullscreen;
	bool running;
	bool vsync;
	i32 width;
	i32 height;
};

extern sp_platform platform_data;