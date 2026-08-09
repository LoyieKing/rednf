#ifndef REACTOR_H_
#define REACTOR_H_

#include "stdafx.h"
#include "ThreadLock.h"
#include "Exception.h"
#include <map>
#include <sys/epoll.h>
#include <unistd.h>

template <class TSession>
class EpollReactor
{
public:
    EpollReactor()
        : epoll_fd_(-1), max_client_(0), events_(NULL)
    {
    }
    ~EpollReactor()
    {
        shutdown();
    }
    bool init(int max_client)
    {
        max_client_ = max_client;
        printf("Epoll init::%d\n", max_client);
        return true;
    }
    void startup()
    {
        if (max_client_ < 1)
        {
            throw Exception(-1, "[EpollReactor< TSession >::Init] Invalid parameter : max_client(%d)\n", max_client_);
        }
        epoll_fd_ = epoll_create(max_client_);
        if (epoll_fd_ == -1)
        {
            printf("[EpollReactor< TSession >::Init] Can't create epoll device : max_client(%d)\n", max_client_);
        }
        events_ = new epoll_event[max_client_];
        if (events_ == NULL)
        {
            printf("[EpollReactor< TSession >::Init] Can't allocate epoll repository : max_client(%d)\n", max_client_);
        }
    }
    void shutdown()
    {
        if (epoll_fd_ == -1)
        {
            return;
        }
        close(epoll_fd_);
        epoll_fd_ = -1;
        if (events_ != NULL)
        {
            ::operator delete[](events_);
        }
    }
    bool registHandle(TSession* s, unsigned int event_filter);
    bool unregistHandle(TSession* s);
    bool handleEvents(unsigned int milisec, bool turn_of_idle);
    bool doSend(TSession* s);
    unsigned int getNativeEventFilter(unsigned int event_filter)
    {
        unsigned int t = 0;
        if ((event_filter & 1) != 0)
        {
            t = t | 1;
        }
        if ((event_filter & 2) != 0)
        {
            t = t | 4;
        }
        if ((event_filter & 4) != 0)
        {
            t = t | 0x18;
        }
        return t;
    }

private:
    std::map<TSession*, unsigned int> map_;
    int epoll_fd_;
    int max_client_;
    epoll_event* events_;
    ThreadLock LockReactor;
};

template <class TReactorImpl, class TSession>
class TReactor
{
public:
    TReactor()
    {
    }
    ~TReactor()
    {
    }
    bool init(int max_client)
    {
        return impl_.init(max_client);
    }
    void startup()
    {
        impl_.startup();
    }
    void shutdown()
    {
        impl_.shutdown();
    }
    bool registHandle(TSession* s, unsigned int event_filter)
    {
        return impl_.registHandle(s, event_filter);
    }
    bool unregistHandle(TSession* s)
    {
        return impl_.unregistHandle(s);
    }
    bool handleEvents(unsigned int milisec, bool turn_of_idle)
    {
        return impl_.handleEvents(milisec, turn_of_idle);
    }
    bool doSend(TSession* s)
    {
        return impl_.doSend(s);
    }

private:
    TReactorImpl impl_;
};

#endif // REACTOR_H_
