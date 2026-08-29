#ifndef MEMPOOL_H_
#define MEMPOOL_H_

#include <string>
#include <vector>

#include "PacketHeader.h"
#include "DNFFileLog.h"

// from GuildMisc.h
// 空闲链表指针存放在对象尾部（sizeof(T)-4）；用成员访问形态让编译器把常量
// 偏移折叠进寻址（ORIG 为 mov 0x84c(%eax),%eax 形态，纯指针运算会产生 lea）。
template<class T>
struct MemPoolFreeLink
{
    char pad[sizeof(T) - 4];
    void* next;
};

template<class T>
class MemPool
{
public:
    MemPool(unsigned int count);
    ~MemPool();
    void* alloc();
    void free(void* p);
    void free(void* p, unsigned int size);
    static void* headOfFreeList_;
    int m_classSize;            // +0
    unsigned int m_count;       // +4
    std::vector<void*> m_chunks; // +8
};

template<class T>
void* MemPool<T>::headOfFreeList_ = 0;

template<class T>
MemPool<T>::MemPool(unsigned int count) : m_classSize((int)sizeof(T)), m_count(count) {}

template<class T>
MemPool<T>::~MemPool()
{
    if (!m_chunks.empty())
    {
        for (std::vector<void*>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
        {
            ::operator delete(*it);
        }
        m_chunks.clear();
    }
}

template<class T>
void* MemPool<T>::alloc()
{
    if (m_classSize != (int)sizeof(T))
    {
        return ::operator new(sizeof(T));
    }
    void* result = headOfFreeList_;
    if (result != 0)
    {
        headOfFreeList_ = ((MemPoolFreeLink<T>*)result)->next;
    }
    else
    {
        MemPoolFreeLink<T>* base = (MemPoolFreeLink<T>*)::operator new(m_count * m_classSize);
        for (unsigned int i = 0; i < m_count - 1U; i++)
        {
            base[i].next = (void*)&base[i + 1];
        }
        base[m_count - 1U].next = 0;
        result = base;
        headOfFreeList_ = (void*)((char*)base + sizeof(T));
        m_chunks.push_back((void*)base);
        DNF_LOG_SCOPE_LINE(0x7d, "./log/Mempool", "class size(%d) cnt(%d)", m_classSize,
            m_count * (int)m_chunks.size());
    }
    return result;
}

template<class T>
void MemPool<T>::free(void* p)
{
    if (p != 0)
    {
        MemPoolFreeLink<T>* t = (MemPoolFreeLink<T>*)p;
        t->next = headOfFreeList_;
        headOfFreeList_ = t;
    }
    return;
}

template<class T>
void MemPool<T>::free(void* p, unsigned int size)
{
    if (p != 0)
    {
        if (m_classSize != size)
        {
            ::operator delete(p);
        }
        else
        {
            MemPoolFreeLink<T>* t = (MemPoolFreeLink<T>*)p;
            t->next = headOfFreeList_;
            headOfFreeList_ = t;
        }
    }
    return;
}

class CUdpRecvBuffer;
class CBlackUser;
class CCashObject;
class CPacketBuffer;
class CTcpRecvBuffer;
class CTcpSendBuffer;
class STPowerWarGuildInfo;
class STPowerWarCharacInfo;
class CPeer;
extern MemPool<CUdpRecvBuffer> m_RecvBufferMemPool_;
extern MemPool<CBlackUser> m_BlackUserMemPool_;
extern MemPool<CCashObject> m_CashObjectMemPool_;
extern MemPool<CPacketBuffer> m_PacketBufferMemPool_;
extern MemPool<CTcpRecvBuffer> m_TcpRecvBufferMemPool_;
extern MemPool<CTcpSendBuffer> m_TcpSendBufferMemPool_;
extern MemPool<STPowerWarGuildInfo> m_PowerWarGuildInfoMemPool_;
extern MemPool<STPowerWarCharacInfo> m_PowerWarCharacInfoMemPool_;
extern MemPool<CPeer> m_PeerMemPool_;

#endif
