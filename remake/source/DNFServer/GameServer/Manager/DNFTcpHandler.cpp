// df_manager_r — CProtocol/EpollHandler/CTcpHandler（ORIG DNFTcpHandler.cpp）
#include "DNFTcpHandler.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>

EpollHandler::EpollHandler()
{
    Init();
}

EpollHandler::~EpollHandler()
{
    Destroy();
}

int EpollHandler::Init()
{
    m_epollFd = epoll_create(0x3e8);
    if (m_epollFd < 0)
    {
        puts("[Epoll::init] Can't init epoll create");
        return 0;
    }
    m_events = (void*)new char[0x2ee0];
    if (!m_events)
    {
        printf("[Epoll::init] Can't alloc event memory");
        return 0;
    }
    return 1;
}

void EpollHandler::Destroy()
{
    if (m_events)
    {
        delete[] (char*)m_events;
        m_events = 0;
    }
}

int EpollHandler::WaitForEvent()
{
    return epoll_wait(GetEpollFD(), (struct epoll_event*)GetEpollEvents(), 0x3e8, 0x64);
}

void* EpollHandler::GetEventPtr(int idx)
{
    return ((struct epoll_event*)m_events)[idx].data.ptr;
}

bool EpollHandler::IsSetInEvent(int idx)
{
    return ((struct epoll_event*)m_events)[idx].events & 0x1;
}

bool EpollHandler::IsSetOutEvent(int idx)
{
    return ((struct epoll_event*)m_events)[idx].events & 0x4;
}

bool EpollHandler::IsSetErrEvent(int idx)
{
    return ((struct epoll_event*)m_events)[idx].events & 0x18;
}

int EpollHandler::SetEpoll(void* peer, int fd, bool flag)
{
    m_eventType = flag ? 0x8000001d : 0x1d;
    m_peer = peer;
    CGuard<CMutex> guard(&m_mutex);
    int ret = epoll_ctl(m_epollFd, 0x1, fd, (struct epoll_event*)&m_eventType);
    return ret < 0 ? errno : 0;
}

int EpollHandler::ResetEpoll(int fd)
{
    memset(&m_eventType, 0, 0xc);
    m_eventType = 0x1;
    CGuard<CMutex> guard(&m_mutex);
    int ret = epoll_ctl(m_epollFd, 0x2, fd, (struct epoll_event*)&m_eventType);
    return ret < 0 ? errno : 0;
}

int EpollHandler::GetEpollFD() { return m_epollFd; }
void* EpollHandler::GetEpollEvents() { return m_events; }

CTcpHandler::CTcpHandler()
{
    m_epoll = new EpollHandler;
}

CTcpHandler::~CTcpHandler()
{
    if (m_epoll)
    {
        delete m_epoll;
        m_epoll = 0;
    }
}

int CTcpHandler::WaitForEvent()
{
    if (m_epoll)
        return m_epoll->WaitForEvent();
    return -1;
}

int CTcpHandler::ResetEpoll(int flag)
{
    if (m_epoll)
        return m_epoll->ResetEpoll(flag);
    return -1;
}

int CTcpHandler::SetPeer(void* peer, int fd, bool flag)
{
    if (m_epoll)
        return m_epoll->SetEpoll(peer, fd, flag);
    return -1;
}

void* CTcpHandler::GetEventPtr(int idx)
{
    if (m_epoll)
        return m_epoll->GetEventPtr(idx);
    return 0;
}

bool CTcpHandler::IsSetInEvent(int idx)
{
    if (m_epoll)
        return m_epoll->IsSetInEvent(idx);
    return 0;
}

bool CTcpHandler::IsSetOutEvent(int idx)
{
    if (m_epoll)
        return m_epoll->IsSetOutEvent(idx);
    return 0;
}

bool CTcpHandler::IsSetErrEvent(int idx)
{
    if (m_epoll)
        return m_epoll->IsSetErrEvent(idx);
    return 0;
}
