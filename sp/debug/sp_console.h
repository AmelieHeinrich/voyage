#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include "../sp_common.h"

#include <imgui/imgui.h>

typedef void(*sp_dev_console_fn)(std::vector<std::string>);

struct sp_dev_console
{
	std::unordered_map<const char*, sp_dev_console_fn> cmds;
	char input_buf[512];
	std::vector<char*> items;
	std::vector<char*> history;
	i32 history_pos;
	bool auto_scroll;
	bool scroll_to_bottom;
};

extern sp_dev_console global_console;

void sp_dev_console_init();
void sp_dev_console_shutdown();
void sp_dev_console_draw(bool* open, bool* focused);
void sp_dev_console_add_log(const char* fmt, ...);