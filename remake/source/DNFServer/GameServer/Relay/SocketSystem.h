#ifndef SOCKETSYSTEM_H_
#define SOCKETSYSTEM_H_

// ---- SocketSystemLinux（全局作用域，空实现）----
class SocketSystemLinux
{
public:
    static void startup();
    static void shutdown();
};

template <class TSystem_>
class TSocketSystem
{
public:
    static void startup()
    {
        TSystem_::startup();
    }
    static void shutdown()
    {
        TSystem_::shutdown();
    }
};

#endif // SOCKETSYSTEM_H_
