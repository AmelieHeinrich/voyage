#pragma once

#include "sp_common.h"

// NOTE(milo): Automatically called when a sp_log fn is called
void sp_console_output(const char* col, const char* msg);
void sp_log_info(const char* fmt, ...);
void sp_log_warn(const char* fmt, ...);
void sp_log_err(const char* fmt, ...);
void sp_log_crit(const char* fmt, ...);