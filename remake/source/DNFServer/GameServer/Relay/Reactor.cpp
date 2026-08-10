// df_relay_r — EpollReactor/TReactor（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include "Reactor.h"
#include "Socket.h"
#include "TCPUser.h"
#include "RelayService.h"
#include "Helper.h"

namespace RelayServiceApp
{

Reactor::Reactor()
{
}

Reactor::~Reactor()
{
}

} // namespace RelayServiceApp

using namespace RelayServiceApp;

template <class TSession, class TSendSocket, class TRecvSocket>
unsigned int EpollReactor<TSession, TSendSocket, TRecvSocket>::getNativeEventFilter(
    unsigned int event_filter)
{
    unsigned int t = 0;
    if ((event_filter & 1) != 0)
    {
        t |= 1;
    }
    if ((event_filter & 2) != 0)
    {
        t |= 4;
    }
    if ((event_filter & 4) != 0)
    {
        t = t | 0x18;
    }
    return t;
}

template <class TSession, class TSendSocket, class TRecvSocket>
bool EpollReactor<TSession, TSendSocket, TRecvSocket>::registHandle(TSession* s,
                                                                    unsigned int event_filter)
{
    epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));
    ev.events = getNativeEventFilter(event_filter);
    ev.data.ptr = s;
    int r = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, s->getHandle(), &ev);
    return r > -1;
}

template <class TSession, class TSendSocket, class TRecvSocket>
bool EpollReactor<TSession, TSendSocket, TRecvSocket>::unregistHandle(TSession* s)
{
    if (s == 0)
    {
        return false;
    }
    if (s->getHandle() == -1 || epoll_fd_ == -1)
    {
        return false;
    }
    epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, s->getHandle(), &ev) < 0)
    {
        return false;
    }
    m_lock.lock();
    typename std::set<TSession*>::iterator it = m_users.find(s);
    if (it == m_users.end())
    {
        m_lock.unlock();
        return false;
    }
    m_users.erase(it);
    m_lock.unlock();
    return true;
}

template <class TSession, class TSendSocket, class TRecvSocket>
bool EpollReactor<TSession, TSendSocket, TRecvSocket>::handleEvents(
    unsigned int milisec, TSendSocket listenSocket, unsigned int maxEvents)
{
    epoll_event ev;
    epoll_event ev2;
    ev.events = 1;
    ev.data.ptr = (void*)listenSocket.getHandle();
    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listenSocket.getHandle(), &ev);
    while (true)
    {
        TSystem<LinuxSystem>::usleep(1);
        int n = epoll_wait(epoll_fd_, events_, max_client_, milisec);
        getManager()->setTick();
        getManager()->makeLog();
        if (n == 0)
        {
        }
        else if (n < 0)
        {
            continue;
        }
        else
        {
            for (int i = 0; i < n; i++)
            {
                if (events_[i].data.ptr == (void*)listenSocket.getHandle())
                {
                    TCPSocket* sock = getManager()->m_userPools.createTCPSocket();
                    if (sock == 0)
                    {
                        WriteLog("[CRITICAL] Reactor.inl: createTCPSocket() Error");
                        continue;
                    }
                    if (!listenSocket.accept(*sock))
                    {
                        WriteLog("Reactor.inl: listenSocket.accept() Error");
                        getManager()->m_userPools.destroyTCPSocket(sock);
                        continue;
                    }
                    if (getManager()->m_users.getUserCount() >=
                        getManager()->m_users.getMaxUserCount())
                    {
                        sock->close();
                        getManager()->m_userPools.destroyTCPSocket(sock);
                        continue;
                    }
                    TCPUser* user = getManager()->m_userPools.createTCPUser();
                    if (user == 0)
                    {
                        WriteLog("Reactor.inl: createTCPUser() Error");
                        sock->close();
                        getManager()->m_userPools.destroyTCPSocket(sock);
                        continue;
                    }
                    getManager()->m_users.increaseUserCount();
                    sock->setOptNonBlock();
                    user->setManager(getManager());
                    user->setSocket(sock);
                    user->startupAfterSetSocket();
                    user->setLastAccessTime();
                    user->onAccept();
                    ev2.events = getNativeEventFilter(maxEvents);
                    ev2.data.ptr = user;
                    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, user->getHandle(), &ev2) < 0)
                    {
                        user->onClose();
                    }
                    else
                    {
                        m_lock.lock();
                        m_users.insert(user);
                        m_lock.unlock();
                    }
                }
                else
                {
                    TCPUser* user = (TCPUser*)events_[i].data.ptr;
                    if (user->getManager() != getManager())
                    {
                        WriteLog("[CRITICAL] Reactor.inl: abnormal connection");
                        close(events_[i].data.fd);
                    }
                    if ((events_[i].events & 8) != 0)
                    {
                        user->onClose();
                        continue;
                    }
                    if ((events_[i].events & 0x10) != 0)
                    {
                        user->onClose();
                        continue;
                    }
                    if ((events_[i].events & 1) != 0)
                    {
                        user->onRead();
                        if (user->isDisconnected())
                        {
                            continue;
                        }
                    }
                    if ((events_[i].events & 4) != 0 && user->isToWrite())
                    {
                        user->onWrite();
                        if (user->isDisconnected())
                        {
                            continue;
                        }
                    }
                    if (user->isAboutToDisconnect())
                    {
                        user->onClose();
                    }
                }
            }
        }
        m_lock.lock();
        bool bAll = true;
        if (m_users.size() != 0)
        {
            typename std::set<TSession*>::const_iterator it = m_users.begin();
            while (it != m_users.end())
            {
                TSession* user = *it;
                if (user == 0)
                {
                    break;
                }
                if (user->isAboutToDisconnect() || user->isIdle())
                {
                    bAll = false;
                    m_lock.unlock();
                    user->onClose();
                    break;
                }
                ++it;
            }
        }
        if (bAll)
        {
            m_lock.unlock();
        }
    }
    return true;
}

// 显式实例化（全局作用域）
template class EpollReactor<RelayServiceApp::TCPUser, TCPSocket, TCPSocket>;
template class TReactor<EpollReactor<RelayServiceApp::TCPUser, TCPSocket, TCPSocket>,
                        RelayServiceApp::TCPUser, TCPSocket, TCPSocket>;
