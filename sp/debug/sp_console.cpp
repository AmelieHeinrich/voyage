#include "sp_console.h"

#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#include <string>

#include "sp_cvar.h"

static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

sp_dev_console global_console;

int TextEditCallback(ImGuiInputTextCallbackData* data)
{
	switch (data->EventFlag)
	{
        case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = global_console.history_pos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (global_console.history_pos == -1)
					global_console.history_pos = (i32)(global_console.history.size()) - 1;
				else if (global_console.history_pos > 0)
					global_console.history_pos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (global_console.history_pos != -1)
					if (++global_console.history_pos >= global_console.history.size())
					global_console.history_pos = -1;
			}
			
			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != global_console.history_pos)
			{
				const char* history_str = (global_console.history_pos >= 0) ? global_console.history[global_console.history_pos] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
			}
		}
	}
	return 0;
}

void sp_dev_console_clear()
{
	for (auto item : global_console.items)
		free(item);
	global_console.items.clear();
}

void sp_dev_console_init()
{
	sp_dev_console_clear();
	memset(global_console.input_buf, 0, sizeof(global_console.input_buf));
	global_console.history_pos = -1;
	global_console.auto_scroll = true;
}

void sp_dev_console_shutdown()
{
	sp_dev_console_clear();
	for (auto history : global_console.history)
		free(history);
}

void sp_dev_console_draw(bool* open, bool* focused)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Developper Console", open))
	{
		ImGui::End();
		return;
	}
	
	*focused = ImGui::IsWindowFocused();
	
	ImGui::Separator();
	
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear")) sp_dev_console_clear();
		ImGui::EndPopup();
	}
	
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
	
	for (auto item : global_console.items)
	{
		ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		if (strstr(item, "[ERROR]")) color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
		if (strstr(item, "[WARN]")) color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
		if (strstr(item, "[INFO]")) color = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
		
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}
	
	if (global_console.scroll_to_bottom || (global_console.auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	global_console.scroll_to_bottom = false;
	
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();
	
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;
	if (ImGui::InputText("Input", global_console.input_buf, 512, input_text_flags, &TextEditCallback))
	{
		char* s = global_console.input_buf;
		sp_dev_console_add_log("> %s", s);
		std::string cmd = std::string(s);
		std::istringstream stream(cmd);
		std::vector<std::string> args;
		
		std::string arg;
		while (stream >> arg)
			args.push_back(arg);
		
		global_console.history_pos = -1;
		for (i32 i = (i32)global_console.history.size() - 1; i >= 0; i--)
		{
			if (Stricmp(global_console.history[i], args[0].c_str()) == 0)
			{
				free(global_console.history[i]);
				global_console.history.erase(global_console.history.begin() + i);
				break;
			}
		}
		
		bool should_find_command = true;
		for (auto cvar = cvar_registry.cvars.begin(); cvar != cvar_registry.cvars.end(); ++cvar)
		{
			if (args[0] == cvar->first)
			{
				if (cvar->second.type == sp_cvar_type::cvar_int)
					cvar_registry.set_i(args[0], std::stoi(args[1]));
				if (cvar->second.type == sp_cvar_type::cvar_float)
					cvar_registry.set_f(args[0], std::stof(args[1]));
				if (cvar->second.type == sp_cvar_type::cvar_string)
					cvar_registry.set_s(args[0], args[1]);
				should_find_command = false;
			}
		}
		
		if (should_find_command)
		{
			bool found_command = false;
			for (auto command = global_console.cmds.begin(); command != global_console.cmds.end(); ++command)
			{
				if (args[0] == command->first)
				{
					global_console.history.push_back(Strdup(command->first));
					found_command = true;
					
					command->second(args);
				}
			}
			if (!found_command)
				sp_dev_console_add_log("Unknown command: %s", args[0].c_str());
		}
		
		global_console.scroll_to_bottom = true;
		strcpy(s, "");
		reclaim_focus = true;
	}
	
	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	
	ImGui::End();
}

void sp_dev_console_add_log(const char* fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 1024, fmt, args);
	buf[1023] = 0;
	va_end(args);
	global_console.items.push_back(Strdup(buf));
}