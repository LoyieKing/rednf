#ifndef TCPTHREAD_H_
#define TCPTHREAD_H_

#include "Thread.h"
#include "Reactor.h"

namespace RelayServiceApp
{
class TCPHandler;
// TCPThread（Thread@0 + TManager@0x14 + port@0x18 + handler@0x1c，0x20）
class TCPThread : public Thread, public TManager<RelayService>
{
public:
    TCPThread();
    ~TCPThread();
    virtual void loop(void* pParam);
    void setPort(int port)
    {
        m_port = port;
    }
    void setHandler(TCPHandler* handler)
    {
        m_handler = handler;
    }
    int getPort() const
    {
        return m_port;
    }
private:
    int m_port;
    TCPHandler* m_handler;
};
} // namespace RelayServiceApp

#endif // TCPTHREAD_H_
