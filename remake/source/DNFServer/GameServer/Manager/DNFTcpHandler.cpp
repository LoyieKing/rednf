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
    m_events = (struct epoll_event*)new char[0x2ee0];
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
        ::operator delete[](m_events);
    }
    m_events = 0;
}

int EpollHandler::WaitForEvent()
{
    struct epoll_event* events = GetEpollEvents();
    int fd = GetEpollFD();
    return epoll_wait(fd, events, 0x3e8, 0x64);
}

void* EpollHandler::GetEventPtr(int idx)
{
    return m_events[idx].data.ptr;
}

bool EpollHandler::IsSetInEvent(int idx)
{
    return m_events[idx].events & 0x1;
}

bool EpollHandler::IsSetOutEvent(int idx)
{
    return m_events[idx].events & 0x4;
}

bool EpollHandler::IsSetErrEvent(int idx)
{
    return m_events[idx].events & 0x18;
}

int EpollHandler::SetEpoll(void* peer, int fd, bool flag)
{
    if (flag)
        m_ev.events = 0x8000001d;
    else
        m_ev.events = 0x1d;
    m_ev.data.ptr = peer;
    CGuard<CMutex> guard(&m_mutex);
    if (epoll_ctl(m_epollFd, 0x1, fd, &m_ev) < 0)
        return errno;
    return 0;
}

int EpollHandler::ResetEpoll(int fd)
{
    memset(&m_ev, 0, 0xc);
    m_ev.events = 0x1;
    CGuard<CMutex> guard(&m_mutex);
    if (epoll_ctl(m_epollFd, 0x2, fd, &m_ev) < 0)
        return errno;
    return 0;
}

int EpollHandler::GetEpollFD() { return m_epollFd; }
struct epoll_event* EpollHandler::GetEpollEvents() { return (struct epoll_event*)m_events; }

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
    if (!m_epoll)
        return -1;
    return m_epoll->WaitForEvent();
}

int CTcpHandler::ResetEpoll(int flag)
{
    if (!m_epoll)
        return -1;
    int ret = m_epoll->ResetEpoll(flag);
    return ret;
}

int CTcpHandler::SetPeer(void* peer, int fd, bool flag)
{
    if (!m_epoll)
        return -1;
    return m_epoll->SetEpoll(peer, fd, flag);
}

void* CTcpHandler::GetEventPtr(int idx)
{
    if (!m_epoll)
        return 0;
    return m_epoll->GetEventPtr(idx);
}

bool CTcpHandler::IsSetInEvent(int idx)
{
    if (!m_epoll)
        return 0;
    return m_epoll->IsSetInEvent(idx);
}

bool CTcpHandler::IsSetOutEvent(int idx)
{
    if (!m_epoll)
        return 0;
    return m_epoll->IsSetOutEvent(idx);
}

bool CTcpHandler::IsSetErrEvent(int idx)
{
    if (!m_epoll)
        return 0;
    return m_epoll->IsSetErrEvent(idx);
}
