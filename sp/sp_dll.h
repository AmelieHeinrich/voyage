#pragma once

#include <Windows.h>

struct dll
{
    HMODULE library;
    char* path;
};

void sp_dll_load(dll* dll, const char* path);
void sp_dll_free(dll* dll);
void* sp_dll_get(dll* dll, const char* fn_name);