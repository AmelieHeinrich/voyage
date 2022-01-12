#pragma once

#include "sp_common.h"
#include <string>

std::string sp_read_file(const char* path);
void sp_write_file(const char* path, void* data, i64 size);