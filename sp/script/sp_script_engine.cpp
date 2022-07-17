#include "sp_script_engine.h"

#include "../sp_log.h"
#include "../sp_input.h"

sp_script_engine script_engine;

int sp_lua_panic(lua_State* L)
{
    longjmp(script_engine.panic_jump, 1);
    return 0;
}

void sp_script_engine_init()
{
    script_engine.state = new sol::state();
    script_engine.state->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
	
    lua_State* L = script_engine.state->lua_state();
    L->l_G->panic = [](lua_State* L)
    {
        sp_log_crit("Script engine panic");
        return 0;
    };
	
    lua_atpanic(L, sp_lua_panic);
}

void sp_script_engine_free()
{
    delete script_engine.state;
    script_engine.state = NULL;
}

void sp_script_engine_register()
{
    auto entity = script_engine.state->new_usertype<sp_entity>("sp_entity");
    entity["name"] = &sp_entity::name;
    entity["id"] = &sp_entity::id;
    entity["volume"] = &sp_entity::volume;
    entity["material_name"] = &sp_entity::material_name;
	
    script_engine.state->set_function("sp_entity_load_mesh", &sp_entity_load_mesh);
    script_engine.state->set_function("sp_entity_load_audio", &sp_entity_load_audio);
    script_engine.state->set_function("sp_entity_play_audio", &sp_entity_play_audio);
    script_engine.state->set_function("sp_entity_stop_audio", &sp_entity_stop_audio);
    script_engine.state->set_function("sp_entity_loop_audio", &sp_entity_loop_audio);
    script_engine.state->set_function("sp_entity_set_material_name", &sp_entity_set_material_name);
    script_engine.state->set_function("sp_entity_set_position", &sp_entity_set_position);
    script_engine.state->set_function("sp_entity_set_scale", &sp_entity_set_scale);
    script_engine.state->set_function("sp_entity_set_rotation", &sp_entity_set_rotation);
	script_engine.state->set_function("sp_entity_set_light_source", &sp_entity_set_light_source);
	script_engine.state->set_function("sp_entity_set_light_color", &sp_entity_set_light_color);
	
    script_engine.state->set_function("sp_key_pressed", &sp_key_pressed);
    script_engine.state->set_function("sp_mouse_pressed", &sp_mouse_pressed);
    script_engine.state->set_function("sp_get_mouse_x", &sp_get_mouse_x);
    script_engine.state->set_function("sp_get_mouse_y", &sp_get_mouse_y);
}

#define LUA_CALL(Namespace, Function, ...) \
if (setjmp(script_engine.panic_jump) == 0)\
{\
lua[Namespace][Function](__VA_ARGS__);\
}\
else\
{\
sp_log_crit("Critical lua error");\
}

void sp_script_engine_load_script(sp_entity* entity, const char* path)
{
    entity->scripted = 1;
    sp_log_info("[INFO] Loading script %s", path);
	
    sol::load_result load_result = script_engine.state->load_file(path);
    if (!load_result.valid())
    {
        sol::error error = load_result;
        sp_log_err("Lua error.");
        sp_log_crit("%s", error.what());
    }
    else
    {
        sol::protected_function_result function_result = load_result();
        if (!function_result.valid())
        {
            sol::error error = function_result;
            sp_log_err("Lua error.");
            sp_log_crit("%s", error.what());
        }   
    }
}

void sp_script_engine_init_entity(sp_entity* entity)
{
    auto& lua = *script_engine.state;
    lua[entity->name]["handle"] = entity;
    LUA_CALL(entity->name, "init");
}

void sp_script_engine_update_entity(sp_entity* entity)
{
    auto& lua = *script_engine.state;
    lua[entity->name]["handle"] = entity;
    LUA_CALL(entity->name, "update");
}

void sp_script_engine_free_entity(sp_entity* entity)
{
    auto& lua = *script_engine.state;
    lua[entity->name]["handle"] = entity;
    LUA_CALL(entity->name, "free");
}
