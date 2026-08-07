#ifndef SOCKETSYSTEM_H_
#define SOCKETSYSTEM_H_

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

#endif // SOCKETSYSTEM_H_
