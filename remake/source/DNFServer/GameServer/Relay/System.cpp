// df_relay_r — LinuxSystem/TSystem（GCC 4.1.2, 无 DWARF）
#include <sys/select.h>

#include "System.h"

void LinuxSystem::sleep(int milisecond)
{
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = milisecond * 1000;
    select(0, 0, 0, 0, &tv);
}

void LinuxSystem::usleep(int microsecond)
{
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = microsecond;
    select(0, 0, 0, 0, &tv);
}

