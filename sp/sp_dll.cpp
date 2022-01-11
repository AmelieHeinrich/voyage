#include "sp_dll.h"

void sp_dll_load(dll* dll, const char* path)
{
    dll->library = LoadLibraryA(path);
    dll->path = (char*)path;
}

void sp_dll_free(dll* dll)
{
    FreeLibrary(dll->library);
}

void* sp_dll_get(dll* dll, const char* fn_name)
{
    return GetProcAddress(dll->library, fn_name);
}