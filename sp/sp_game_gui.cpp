#include "sp_game_gui.h"

#include "sp_game.h"
#include "sp_log.h"
#include "debug/sp_console.h"
#include "debug/sp_cvar.h"
#include <imgui/imgui.h>

sp_cvar* showpos;

void sp_init_game_gui()
{
	showpos = cvar_registry.get("sp_showpos");
}

void sp_showpos()
{
	static int corner = 0;
	static bool show = true;
	
	if (showpos->as.i)
	{
		sp_game_state* state = sp_get_game_state();
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
		
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        
		ImGui::Begin("Overlay", &show, window_flags);
		ImGui::Text("pos: %.3f %.3f %.3f", state->cam.position.x, state->cam.position.y, state->cam.position.z);
		ImGui::Text("vel: %.3f %.3f %.3f", state->cam.velocity.x, state->cam.velocity.y, state->cam.velocity.z);
		ImGui::Text("ang: %.3f %.3f", state->cam.yaw, state->cam.pitch);
		ImGui::End();
	}
}