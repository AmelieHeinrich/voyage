#include "sp_timer.h"

#include <Windows.h>

struct sp_timer
{
    i64 start;
    i64 freq;
};

sp_timer timer;

void sp_timer_init()
{
    LARGE_INTEGER large;

    QueryPerformanceCounter(&large);
    timer.start = large.QuadPart;

    QueryPerformanceFrequency(&large);
    timer.freq = large.QuadPart;
}

f32 sp_timer_get()
{
    LARGE_INTEGER large_int;
    QueryPerformanceCounter(&large_int);

    i64 now = large_int.QuadPart;
    i64 time = now - timer.start;

    return (f32)((f64)time / timer.freq);
}