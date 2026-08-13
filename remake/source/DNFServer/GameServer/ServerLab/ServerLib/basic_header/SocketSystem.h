#ifndef NSL_SOCKETSYSTEM_H_
#define NSL_SOCKETSYSTEM_H_

namespace nsl {

class SocketSystemLinux
{
public:
    static void startup();
    static void shutdown();
};

template <class TSocketSystem_>
class TSocketSystem
{
public:
    static void startup()
    {
        TSocketSystem_::startup();
    }
    static void shutdown()
    {
        TSocketSystem_::shutdown();
    }
};

typedef TSocketSystem<SocketSystemLinux> SocketSystem;

} // namespace nsl

#endif // NSL_SOCKETSYSTEM_H_
