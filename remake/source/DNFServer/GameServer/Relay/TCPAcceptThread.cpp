// df_relay_r — TCPAcceptThread（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include "TCPAcceptThread.h"
#include "TCPUser.h"
#include "Socket.h"
#include "RelayService.h"

namespace RelayServiceApp
{
TCPAcceptThread::TCPAcceptThread()
{
    m_port = 0;
}

TCPAcceptThread::~TCPAcceptThread()
{
}

void TCPAcceptThread::loop(void* pParam)
{
    unsigned short port = (unsigned short)getPort();
    printf("In %s \n", __FUNCTION__);
    TCPSocket listenSocket;
    if (!listenSocket.open())
    {
        throw Exception("Fail to open socket");
    }
    if (!listenSocket.bind(port, false))
    {
        throw Exception("Fail to bind listen");
    }
    if (!listenSocket.listen(100))
    {
        throw Exception("Fail to Create AcceptThread.");
    }
    while (!isTerminating())
    {
        TSystem<LinuxSystem>::usleep(100);
        if (isStop())
        {
            break;
        }
        TCPSocket* sock = getManager()->m_userPools.createTCPSocket();
        if (sock == 0)
        {
            continue;
        }
        bool accepted = listenSocket.accept(*sock);
        if (!accepted)
        {
            getManager()->m_userPools.destroyTCPSocket(sock);
            continue;
        }
        if (getManager()->m_users.getUserCount() >= getManager()->m_users.getMaxUserCount())
        {
            notifyCannotLoginByMaxUserCount(*sock);
            sock->close();
            getManager()->m_userPools.destroyTCPSocket(sock);
            continue;
        }
        TCPUser* user = getManager()->m_userPools.createTCPUser();
        if (user == 0)
        {
            notifyCannotCreateUser(*sock);
            sock->close();
            getManager()->m_userPools.destroyTCPSocket(sock);
            continue;
        }
        user->setManager(getManager());
        user->setSocket(sock);
        user->startupAfterSetSocket();
        lockPushAcceptedUser(user);
        continue;
    }
    isTerminating();
    listenSocket.close();
    setTerminated();
}

void TCPAcceptThread::lockPushAcceptedUser(TCPUser* user)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock);
    m_acceptedUsers.push(user);
}

TCPUser* TCPAcceptThread::lockPopAcceptedUser()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock);
    if (m_acceptedUsers.empty())
    {
        return 0;
    }
    TCPUser* user = m_acceptedUsers.front();
    m_acceptedUsers.pop();
    return user;
}

void TCPAcceptThread::notifyCannotCreateUser(TCPSocket& sock)
{
}

void TCPAcceptThread::notifyCannotLoginByMaxUserCount(TCPSocket& sock)
{
}


} // namespace RelayServiceApp
