#include "sp_game_gui.h"

#include "sp_game.h"
#include "sp_log.h"
#include "debug/sp_console.h"
#include <imgui/imgui.h>

bool show_pos = false;

void sp_enable_showpos(std::vector<std::string> args)
{
	show_pos = !show_pos;
}

void sp_enable_skybox(std::vector<std::string> args)
{
	sp_get_game_state()->enable_skybox = !sp_get_game_state()->enable_skybox;
}

void sp_enable_fxaa(std::vector<std::string> args)
{
	sp_get_game_state()->enable_fxaa = !sp_get_game_state()->enable_fxaa;
}

void sp_set_render_mode(std::vector<std::string> args)
{
	int mode = std::stoi(args[1]);
	if (mode < 0 || mode > 7)
		return;
	
	sp_get_game_state()->current_scene.scene_render.render_mode = mode;
}

void sp_set_wireframe(std::vector<std::string> args)
{
	int wireframe = std::stoi(args[1]);
	sp_get_game_state()->current_scene.scene_render.force_wireframe = (bool)wireframe;
}

void sp_showpos()
{
	static int corner = 0;
	static bool show = true;
	
	if (show_pos)
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