#ifndef UDPHANDLER_H_
#define UDPHANDLER_H_

#include "Reactor.h"

namespace RelayServiceApp
{
class RelayService;
// UDPHandler / UDPHandlerRelay / UDPHandlerS2S（派生 vptr@0 + TManager@4，8B）
class UDPHandler
{
public:
    UDPHandler();
    virtual void dispatch(char* buf, int size, int flag) = 0;
};

class UDPHandlerRelay : public TManager<RelayService>, public UDPHandler
{
public:
    UDPHandlerRelay();
    ~UDPHandlerRelay();
    virtual void dispatch(char* buf, int size, int flag);
};

class UDPHandlerS2S : public TManager<RelayService>, public UDPHandler
{
public:
    UDPHandlerS2S();
    ~UDPHandlerS2S();
    virtual void dispatch(char* buf, int size, int flag);
};
} // namespace RelayServiceApp

#endif // UDPHANDLER_H_
