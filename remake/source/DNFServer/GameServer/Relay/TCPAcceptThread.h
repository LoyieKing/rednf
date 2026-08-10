#ifndef TCPACCEPTTHREAD_H_
#define TCPACCEPTTHREAD_H_

#include <queue>
#include "Thread.h"
#include "Reactor.h"
#include "ThreadLock.h"

class TCPSocket;

namespace RelayServiceApp
{
class TCPUser;
// TCPAcceptThread（Thread@0 + TManager@0x14 + queue@0x18 + lock@0x40 +
//      port@0x58，0x5c）
class TCPAcceptThread : public Thread, public TManager<RelayService>
{
public:
    TCPAcceptThread();
    ~TCPAcceptThread();
    virtual void loop(void* pParam);
    void lockPushAcceptedUser(TCPUser* user);
    TCPUser* lockPopAcceptedUser();
    void notifyCannotCreateUser(TCPSocket& sock);
    void notifyCannotLoginByMaxUserCount(TCPSocket& sock);
    int getPort() const
    {
        return m_port;
    }
private:
    std::queue<TCPUser*> m_acceptedUsers;
    TThreadLock<ThreadLock_linux> m_lock;
    int m_port;
};
} // namespace RelayServiceApp

#endif // TCPACCEPTTHREAD_H_
