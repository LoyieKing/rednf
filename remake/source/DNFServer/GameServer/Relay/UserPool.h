#ifndef USERPOOL_H_
#define USERPOOL_H_

#include <queue>
#include <deque>
#include "Socket.h"
#include "TCPUser.h"
#include "UDPUser.h"


#include "Exception.h"

// 对象池：maxCount@0 / array_@4 / queue<T*>@8（0x30）
template <class T, class TQueue>
class TMemoryPoolStatic
{
public:
#ifdef RELAY_USERPOOL_C6
    TMemoryPoolStatic() : maxCount_(0), queue_(std::deque<T*>())
    {
        array_ = 0;
    }
#else
    TMemoryPoolStatic() : maxCount_(0), array_(0)
    {
    }
#endif
    ~TMemoryPoolStatic()
    {
        shutdown();
    }
    void startup(int maxCount)
    {
        if (array_ != 0)
        {
            throw Exception("repository_ not NULL");
        }
        maxCount_ = maxCount;
        T* p = new T[maxCount];
        for (int i = 0; i < maxCount; i++)
        {
            queue_.push(&p[i]);
        }
        array_ = p;
    }
    T* alloc()
    {
        if (queue_.empty())
        {
            return 0;
        }
        T* p = queue_.front();
        queue_.pop();
        return p;
    }
    void free(T* p)
    {
        if (p == 0)
        {
            throw Exception("cannot free NULL");
        }
        if (array_ == 0)
        {
            throw Exception("repository NULL");
        }
        long long index = 0;
        long long diff = ((char*)p - (char*)array_) / 4 / 7;
        if (diff > 0)
        {
            index = diff / (long long)sizeof(T);
        }
        if (maxCount_ > index && index >= 0)
        {
            queue_.push(p);
        }
    }
    void shutdown()
    {
        if (array_ != 0)
        {
            delete[] array_;
            array_ = 0;
        }
    }

private:
    int maxCount_;
    T* array_;
    TQueue queue_;
};


namespace RelayServiceApp
{
// ---- UserPools：3 个 0x30 对象池（0x90）----
class UserPools
{
public:
    UserPools();
    ~UserPools();
    TCPSocket* createTCPSocket();
    void destroyTCPSocket(TCPSocket* sock);
    TCPUser* createTCPUser();
    void destroyTCPUser(TCPUser* user);
    UDPUser* createUDPUser();
    void destroyUDPUser(UDPUser* user);

    TMemoryPoolStatic<TCPSocket, std::queue<TCPSocket*, std::deque<TCPSocket*, std::allocator<TCPSocket*> > > > m_tcpSocketPool;   // +0
    TMemoryPoolStatic<TCPUser, std::queue<TCPUser*, std::deque<TCPUser*, std::allocator<TCPUser*> > > > m_tcpUserPool;             // +0x30
    TMemoryPoolStatic<UDPUser, std::queue<UDPUser*, std::deque<UDPUser*, std::allocator<UDPUser*> > > > m_udpUserPool;             // +0x60
};


} // namespace RelayServiceApp

#endif // USERPOOL_H_
