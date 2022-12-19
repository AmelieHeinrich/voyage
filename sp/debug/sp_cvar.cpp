#include "sp_cvar.h"

sp_cvar_registry cvar_registry;

void sp_cvar_registry::add_i(std::string name, i32 var)
{
    cvar_registry.cvars[name].type = sp_cvar_type::cvar_int;
    cvar_registry.cvars[name].as.i = var;
}

void sp_cvar_registry::add_f(std::string name, f32 var)
{
    cvar_registry.cvars[name].type = sp_cvar_type::cvar_float;
    cvar_registry.cvars[name].as.f = var;
}

void sp_cvar_registry::add_s(std::string name, std::string var)
{
    cvar_registry.cvars[name].type = sp_cvar_type::cvar_string;
    cvar_registry.cvars[name].as.s = var;
}

sp_cvar* sp_cvar_registry::get(std::string name)
{
    return &cvar_registry.cvars[name];
}

void sp_cvar_registry::set_i(std::string name, i32 var)
{
    assert(cvar_registry.cvars[name].type == sp_cvar_type::cvar_int);
    cvar_registry.cvars[name].as.i = var;
}

void sp_cvar_registry::set_f(std::string name, f32 var)
{
    assert(cvar_registry.cvars[name].type == sp_cvar_type::cvar_float);
    cvar_registry.cvars[name].as.f = var;
}

void sp_cvar_registry::set_s(std::string name, std::string var)
{
    assert(cvar_registry.cvars[name].type == sp_cvar_type::cvar_string);
    cvar_registry.cvars[name].as.s = var;
}

void sp_init_cvar_registry()
{
    cvar_registry.add_i("sp_showpos", 0);
    cvar_registry.add_i("mat_toggle_fxaa", 1);
    cvar_registry.add_i("mat_toggle_env", 1);
    cvar_registry.add_i("mat_render_mode", 0);
    cvar_registry.add_i("mat_wireframe", 0);
    cvar_registry.add_i("mat_toggle_bloom", 1);
    cvar_registry.add_f("mat_bloom_threshold", 0.7f);
}

void sp_free_cvar_registry()
{
    cvar_registry.cvars.clear();
}