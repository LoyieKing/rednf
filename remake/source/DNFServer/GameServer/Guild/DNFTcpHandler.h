#ifndef DNFTCPHANDLER_H_
#define DNFTCPHANDLER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"
#include <sys/epoll.h>

#include "PacketHeader.h"
#include "DNFTcpHandler.h"

class CProtocol;
class CTcpHandler;
class EpollHandler;

// from GuildDomain.h
class CTcpHandler
{
public:
    CTcpHandler();
    ~CTcpHandler();
    int SetPeer(void* ptr, int fd, bool flag);
    int ResetEpoll(int fd);
    int WaitForEvent();
    bool IsSetErrEvent(int idx);
    bool IsSetOutEvent(int idx);
    bool IsSetInEvent(int idx);
    void* GetEventPtr(int idx);
    EpollHandler* m_epoll;  // +0
};

// from GuildDomain.h
class CProtocol
{
public:
    CProtocol();
    virtual ~CProtocol();
    virtual int Init() = 0;
    virtual int SetEpoll(void* ptr, int fd, bool flag) = 0;
    virtual int ResetEpoll(int fd) = 0;
    virtual void Destroy() = 0;
    virtual int WaitForEvent() = 0;
    virtual bool IsSetErrEvent(int idx) = 0;
    virtual bool IsSetOutEvent(int idx) = 0;
    virtual bool IsSetInEvent(int idx) = 0;
    virtual void* GetEventPtr(int idx) = 0;
};

// from GuildDomain.h
class EpollHandler : public CProtocol
{
public:
    EpollHandler();
    virtual ~EpollHandler();
    virtual int Init();
    virtual int SetEpoll(void* ptr, int fd, bool flag);
    virtual int ResetEpoll(int fd);
    virtual void Destroy();
    virtual int WaitForEvent();
    virtual bool IsSetErrEvent(int idx);
    virtual bool IsSetOutEvent(int idx);
    virtual bool IsSetInEvent(int idx);
    virtual void* GetEventPtr(int idx);
    int GetEpollFD();
    void* GetEpollEvents();
    int m_field4;     // +4
    void* m_ptr;      // +8
    int m_fieldC;     // +0xc
    epoll_event* m_events;    // +0x10
    int m_epollFd;    // +0x14
    CMutex m_mutex;   // +0x18
};

#endif
