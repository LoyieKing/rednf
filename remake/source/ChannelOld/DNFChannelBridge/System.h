#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "stdafx.h"
#include <sys/time.h>

typedef long long __int64;

class LinuxSystem
{
public:
    static void sleep(int milisecond);
    static void usleep(int microsecond);
};

template <class TSystem_>
class TSystem
{
public:
    static void sleep(int n)
    {
        TSystem_::sleep(n);
    }
    static void usleep(int n)
    {
        TSystem_::usleep(n);
    }
};

__int64 get_ms_tick();

#endif // SYSTEM_H_
