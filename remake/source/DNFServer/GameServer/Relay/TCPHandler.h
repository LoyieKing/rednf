#ifndef TCPHANDLER_H_
#define TCPHANDLER_H_

#include "Reactor.h"

namespace RelayServiceApp
{
class TCPUser;
class RelayService;
// TCPHandler / TCPHandlerRelay（派生 vptr@0 + TManager@4，8B）
class TCPHandler
{
public:
    TCPHandler();
    virtual void dispatch(TCPUser* user, char* buf, int size, int flag) = 0;
};

class TCPHandlerRelay : public TManager<RelayService>, public TCPHandler
{
public:
    TCPHandlerRelay();
    ~TCPHandlerRelay();
    virtual void dispatch(TCPUser* user, char* buf, int size, int flag);
};
} // namespace RelayServiceApp

#endif // TCPHANDLER_H_
