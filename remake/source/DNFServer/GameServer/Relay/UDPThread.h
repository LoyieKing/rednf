#ifndef UDPTHREAD_H_
#define UDPTHREAD_H_

#include "Thread.h"
#include "Reactor.h"
#include "Socket.h"

namespace RelayServiceApp
{
class UDPHandler;
// UDPThread（Thread@0 + TManager@0x14 + handler@0x18 + port@0x1c +
//      udpSocket@0x20 + tick@0x24，0x2c）
class UDPThread : public Thread, public TManager<RelayService>
{
public:
    UDPThread();
    ~UDPThread();
    virtual void loop(void* pParam);
    void setPort(int port)
    {
        m_port = port;
    }
    void setHandler(UDPHandler* handler)
    {
        m_handler = handler;
    }
    int getPort() const
    {
        return m_port;
    }
    UDPSocket* getUDPSocket()
    {
        return m_udpSocket;
    }
    void setUDPSocket(UDPSocket* sock)
    {
        m_udpSocket = sock;
    }
    void logError();
private:
    UDPHandler* m_handler;
    int m_port;
    UDPSocket* m_udpSocket;
    long long m_tick;
};
} // namespace RelayServiceApp

#endif // UDPTHREAD_H_
