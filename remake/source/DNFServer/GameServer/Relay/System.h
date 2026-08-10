#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <sys/select.h>

// ---- Thread：vptr@0 / handle_@4 / id_@8 / states_@0xc，总 20B ----

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

#endif // SYSTEM_H_
