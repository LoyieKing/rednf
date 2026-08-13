#ifndef NSL_DISPATCHERS_H_
#define NSL_DISPATCHERS_H_

namespace nsl {

class UDPDispatcher;
class TCPDispatcher;
class InterDispatcher;
class DBDispatcher;

class Dispatchers
{
public:
    Dispatchers()
    {
        dispatcherUDP = NULL;
        dispatcherTCP = NULL;
        mpInterDispatcher = NULL;
        dispatcherDB = NULL;
    }
    UDPDispatcher* getUDPDispatcher()
    {
        return dispatcherUDP;
    }
    TCPDispatcher* getTCPDispatcher()
    {
        return dispatcherTCP;
    }
    InterDispatcher* GetInterDispatcher()
    {
        return mpInterDispatcher;
    }
    DBDispatcher* getDBDispatcher()
    {
        return dispatcherDB;
    }

    UDPDispatcher* dispatcherUDP;
    TCPDispatcher* dispatcherTCP;
    InterDispatcher* mpInterDispatcher;
    DBDispatcher* dispatcherDB;
};

} // namespace nsl

#endif // NSL_DISPATCHERS_H_
