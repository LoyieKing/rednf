#include <sys/select.h>

#include "System.h"

void LinuxSystem::sleep(int milisecond)
{
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = milisecond * 1000;
    select(0, NULL, NULL, NULL, &tv);
}

void LinuxSystem::usleep(int microsecond)
{
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = microsecond;
    select(0, NULL, NULL, NULL, &tv);
}

__int64 get_ms_tick()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return (__int64)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// --- Functions defined in this compilation unit ---

// get_ms_tick()
/* TODO: implement */

// LinuxSystem::sleep(int)
/* TODO: implement */

// LinuxSystem::usleep(int)
/* TODO: implement */
