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
        puts("epoll create error");
        return 0;
    }
    m_events = (void*)new char[0x2ee0];
    if (!m_events)
    {
        printf("epoll events alloc error\n");
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
char EpollHandler::IsSetInEvent(int idx)
{
    return ((struct epoll_event*)m_events)[idx].events & 0x1;
}
char EpollHandler::IsSetOutEvent(int idx)
{
    return (((struct epoll_event*)m_events)[idx].events & 0x4) != 0;
}
char EpollHandler::IsSetErrEvent(int idx)
{
    return (((struct epoll_event*)m_events)[idx].events & 0x18) != 0;
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
