#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <sol/lstate.h>

#include "../entity/sp_entity.h"

struct sp_script_engine
{
    sol::state* state;
    jmp_buf panic_jump;
};

void sp_script_engine_init();
void sp_script_engine_free();
void sp_script_engine_register();
void sp_script_engine_load_entity(const char* path);

void sp_script_engine_load_script(sp_entity* entity, const char* path);
void sp_script_engine_init_entity(sp_entity* entity);
void sp_script_engine_update_entity(sp_entity* entity);
void sp_script_engine_free_entity(sp_entity* entity);
