// df_game_r EpollHandler 实现（ORIG 0x857c084-0x857c2e3，AE 口径还原）。
#include "EpollHandler.h"

#include <errno.h>
#include <ostream>
#include <string.h>

namespace std
{
extern ostream cerr;   // 仅声明（真实定义在 libstdc++），避免 TU 内 __ioinit 静态初始化
}

EpollHandler::EpollHandler()
{
    init();
}

EpollHandler::~EpollHandler()
{
    destroy();
    release();
}

bool EpollHandler::init()
{
    m_fd = epoll_create(600);
    if (m_fd < 0)
    {
        std::cerr << "[Epoll::init] Can't init epoll create" << std::endl;
        return 0;
    }

    m_events = (epoll_event*)new char[0x1c20];
    if (m_events == NULL)
    {
        std::cerr << "[Epoll::init] Can't alloc event memory" << std::endl;
        return 0;
    }
    return 1;
}

void EpollHandler::destroy()
{
    if (m_events != NULL)
    {
        ::operator delete[](m_events);
    }
    m_events = NULL;
}

void EpollHandler::release()
{
}

int EpollHandler::SetEpoll(void* data, int fd, bool bET)
{
    if (bET)
    {
        m_event.events = 0x8000001d;
    }
    else
    {
        m_event.events = 0x1d;
    }
    m_event.data.ptr = data;

    Guard<Mutex> guard(&m_mutex);
    if (epoll_ctl(m_fd, EPOLL_CTL_ADD, fd, &m_event) < 0)
    {
        return errno;
    }
    return 0;
}

int EpollHandler::ResetEpoll(int fd)
{
    memset(&m_event, 0, sizeof(m_event));
    m_event.events = EPOLLIN;

    Guard<Mutex> guard(&m_mutex);
    if (epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, &m_event) < 0)
    {
        return errno;
    }
    return 0;
}

int EpollHandler::GetEpollFD()
{
    return m_fd;
}

epoll_event* EpollHandler::GetEpollEvents()
{
    return m_events;
}
