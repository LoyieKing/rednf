#ifndef DBMW_EPOLLHANDLER_H_
#define DBMW_EPOLLHANDLER_H_

#include "DBMWCommon.h"


class CProtocol
{
public:
    CProtocol() {}
    virtual ~CProtocol() {}
    virtual int Init() = 0;
    virtual int SetEpoll(void* peer, int fd, bool flag) = 0;
    virtual int ResetEpoll(int fd) = 0;
    virtual void Destroy() = 0;
    virtual int WaitForEvent() = 0;
    virtual bool IsSetErrEvent(int idx) = 0;
    virtual bool IsSetOutEvent(int idx) = 0;
    virtual bool IsSetInEvent(int idx) = 0;
    virtual void* GetEventPtr(int idx) = 0;  // ORIG vtable +0x28（经 CProtocol* 调用）
};

class EpollHandler : public CProtocol
{
public:
    EpollHandler();
    virtual ~EpollHandler();
    virtual int Init();
    virtual int SetEpoll(void* peer, int fd, bool flag);
    virtual int ResetEpoll(int fd);
    virtual void Destroy();
    virtual int WaitForEvent();
    virtual bool IsSetErrEvent(int idx);
    virtual bool IsSetOutEvent(int idx);
    virtual bool IsSetInEvent(int idx);
    virtual void* GetEventPtr(int idx);
    int GetEpollFD();
    struct epoll_event* GetEpollEvents();
    int m_eventType;  // +4
    void* m_peer;     // +8
    char m_dataC[4];  // +0xc
    struct epoll_event* m_events;  // +0x10（指针，布局不变）
    int m_epollFd;    // +0x14
    CMutex m_mutex;   // +0x18
};

#endif  // DBMW_EPOLLHANDLER_H_
