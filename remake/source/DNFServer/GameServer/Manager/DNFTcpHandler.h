#ifndef DNF_TCP_HANDLER_H_
#define DNF_TCP_HANDLER_H_

#include "Thread.h"
#include <sys/epoll.h>

// ---- CProtocol / EpollHandler / CTcpHandler ----
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
    epoll_event m_ev; // +4（events+data，12 字节）
    struct epoll_event* m_events;  // +0x10（指针，布局不变）
    int m_epollFd;    // +0x14
    CMutex m_mutex;   // +0x18
};

class CTcpHandler
{
public:
    CTcpHandler();
    ~CTcpHandler();
    int WaitForEvent();
    int ResetEpoll(int flag);
    int SetPeer(void* peer, int fd, bool flag);
    void* GetEventPtr(int idx);
    bool IsSetInEvent(int idx);
    bool IsSetOutEvent(int idx);
    bool IsSetErrEvent(int idx);
    CProtocol* m_epoll;  // +0（ORIG：基类指针赋值产生 mov ebx,eax;mov eax,edx 形态）
};

#endif  // DNF_TCP_HANDLER_H_
