#ifndef NSL_SYSTEM_H_
#define NSL_SYSTEM_H_

#include <stddef.h>
#include <sys/time.h>

namespace nsl {

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
};

long long get_ms_tick();

} // namespace nsl

#endif // NSL_SYSTEM_H_
