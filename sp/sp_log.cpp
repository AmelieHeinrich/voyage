#include "sp_log.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#define LOG_BUF_SIZE 1024

#define LOG_RESET   "\033[0m"
#define LOG_BLACK   "\033[30m"     
#define LOG_RED     "\033[31m"     
#define LOG_GREEN   "\033[32m"     
#define LOG_YELLOW  "\033[33m"     
#define LOG_BLUE    "\033[34m"     
#define LOG_MAGENTA "\033[35m"     
#define LOG_CYAN    "\033[36m"     
#define LOG_WHITE   "\033[37m"     
#define LOG_BOLD_BLACK   "\033[1m\033[30m"      
#define LOG_BOLD_RED     "\033[1m\033[31m"      
#define LOG_BOLD_GREEN   "\033[1m\033[32m"      
#define LOG_BOLD_YELLOW  "\033[1m\033[33m"      
#define LOG_BOLD_BLUE    "\033[1m\033[34m"      
#define LOG_BOLD_MAGENTA "\033[1m\033[35m"      
#define LOG_BOLD_CYAN    "\033[1m\033[36m"      
#define LOG_BOLD_WHITE   "\033[1m\033[37m"     

void sp_console_output(const char* col, const char* msg)
{
    printf(LOG_RESET);
    printf(col);
    printf("%s\n", msg);
    printf(LOG_RESET);
}

void sp_log_info(const char* fmt, ...)
{
    char buf[LOG_BUF_SIZE];    
    va_list vl;
    va_start(vl, fmt);

    vsnprintf(buf, sizeof(buf), fmt, vl);

    va_end(vl);

    sp_console_output(LOG_CYAN, buf);
}

void sp_log_warn(const char* fmt, ...)
{
    char buf[LOG_BUF_SIZE];    
    va_list vl;
    va_start(vl, fmt);

    vsnprintf(buf, sizeof(buf), fmt, vl);

    va_end(vl);

    sp_console_output(LOG_YELLOW, buf);
}

void sp_log_err(const char* fmt, ...)
{
    char buf[LOG_BUF_SIZE];    
    va_list vl;
    va_start(vl, fmt);

    vsnprintf(buf, sizeof(buf), fmt, vl);

    va_end(vl);
    
    sp_console_output(LOG_RED, buf);
}

void sp_log_crit(const char* fmt, ...)
{
    char buf[LOG_BUF_SIZE];    
    va_list vl;
    va_start(vl, fmt);

    vsnprintf(buf, sizeof(buf), fmt, vl);

    va_end(vl);

    sp_console_output(LOG_RED, buf);
    assert(false && buf);
}