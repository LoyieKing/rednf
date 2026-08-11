#include <sys/select.h>

#include "System.h"

namespace nsl {

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

long long get_ms_tick()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    long long sec;
    sec = tv.tv_sec;
    return sec * 1000 + (unsigned long long)tv.tv_usec / 1000;
}

} // namespace nsl
