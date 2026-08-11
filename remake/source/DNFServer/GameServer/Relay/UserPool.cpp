// df_relay_r — UserPools/对象池 TU（原版该 TU 由 GCC 4.4.6-3 编译：
//   queue(deque&&) rvalue 构造 + 4.4 头文件实例化）
#include <deque>
#include <queue>

#include "UserPool.h"
#include "RelayService.h"
#include "TCPUser.h"
#include "UDPUser.h"
#include "Socket.h"



namespace RelayServiceApp
{

UserPools::UserPools()
{
}

UserPools::~UserPools()
{
}

TCPSocket* UserPools::createTCPSocket()
{
    TCPSocket* p = m_tcpSocketPool.alloc();
    return p;
}

void UserPools::destroyTCPSocket(TCPSocket* sock)
{
    m_tcpSocketPool.free(sock);
}

TCPUser* UserPools::createTCPUser()
{
    TCPUser* p = m_tcpUserPool.alloc();
    return p;
}

void UserPools::destroyTCPUser(TCPUser* user)
{
    TCPSocket* sock = user->getSocket();
    if (sock != 0)
    {
        sock->close();
        destroyTCPSocket(sock);
    }
    user->setSocket(0);
    user->setACCID(0);
    m_tcpUserPool.free(user);
}

UDPUser* UserPools::createUDPUser()
{
    UDPUser* p = m_udpUserPool.alloc();
    return p;
}

void UserPools::destroyUDPUser(UDPUser* user)
{
    m_udpUserPool.free(user);
}

} // namespace RelayServiceApp

// 显式实例化对象池全套 API（原版 UserPool TU 由 4.4.6-3 编译，含 queue(deque&&)/push(&&)）
template class TMemoryPoolStatic<
    TCPSocket, std::queue<TCPSocket*, std::deque<TCPSocket*, std::allocator<TCPSocket*> > > >;
template class TMemoryPoolStatic<
    RelayServiceApp::TCPUser,
    std::queue<RelayServiceApp::TCPUser*,
               std::deque<RelayServiceApp::TCPUser*, std::allocator<RelayServiceApp::TCPUser*> > > >;
template class TMemoryPoolStatic<
    RelayServiceApp::UDPUser,
    std::queue<RelayServiceApp::UDPUser*,
               std::deque<RelayServiceApp::UDPUser*, std::allocator<RelayServiceApp::UDPUser*> > > >;
