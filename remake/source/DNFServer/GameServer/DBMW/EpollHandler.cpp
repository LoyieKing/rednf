// df_dbmw_r - EpollHandler (ORIG EpollHandler.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

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
struct epoll_event* EpollHandler::GetEpollEvents() { return m_events; }
