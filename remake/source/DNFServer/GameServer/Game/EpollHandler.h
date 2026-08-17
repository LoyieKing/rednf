#ifndef GAME_EPOLLHANDLER_H_
#define GAME_EPOLLHANDLER_H_

#include <sys/epoll.h>

#include "GameBase.h"

// df_game_r EpollHandler（ORIG 0x857c084，0x2c 字节）。
// 布局：epoll_event @ +0x00（0xc），events* @ +0x0c，fd @ +0x10，Mutex @ +0x14。
class EpollHandler
{
public:
    EpollHandler();
    ~EpollHandler();

    bool init();
    void destroy();
    void release();
    int SetEpoll(void* data, int fd, bool bET);
    int ResetEpoll(int fd);
    int GetEpollFD();
    epoll_event* GetEpollEvents();

private:
    epoll_event m_event;      // +0x00
    epoll_event* m_events;    // +0x0c
    int m_fd;                 // +0x10
    Mutex m_mutex;            // +0x14
};

#endif  // GAME_EPOLLHANDLER_H_
