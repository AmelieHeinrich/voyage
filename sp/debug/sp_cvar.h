#pragma once

#include <string>
#include <unordered_map>
#include <assert.h>
#include "../sp_common.h"

enum class sp_cvar_type : char
{
    cvar_int,
    cvar_float,
    cvar_string
};

struct sp_cvar
{
    sp_cvar_type type;
    struct {
        i32 i;
        f32 f;
        std::string s;
    } as;
};

struct sp_cvar_registry
{
    std::unordered_map<std::string, sp_cvar> cvars;
    
    void add_i(std::string name, i32 var);
    void add_f(std::string name, f32 var);
    void add_s(std::string name, std::string var);
    
    sp_cvar* get(std::string name);
    void set_i(std::string name, i32 value);
    void set_f(std::string name, f32 value);
    void set_s(std::string name, std::string value);
};

extern sp_cvar_registry cvar_registry;

void sp_init_cvar_registry();
void sp_free_cvar_registry();