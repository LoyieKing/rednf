// df_manager_r — CUdpRecvBuffer/CTcpRecvBuffer/CTcpSendBuffer/CPacketBuffer + MemPool/IQueue/CSwapQueue
//               （ORIG DNFPacketBuffer.cpp）
#include "DNFPacketBuffer.h"

#include "DNFFileLog.h"
#include "Peer.h"
#include "ProhibitUser.h"

template<class T> void* MemPool<T>::headOfFreeList_;

// 空闲链表链接点：每个池对象尾部 4 字节（成员位移形态，与 ORIG 逐条一致）。
template<class T>
struct MemPoolFreeLink
{
    char pad[sizeof(T) - 4];
    void* next;
};

template<class T>
MemPool<T>::MemPool() {}

template<class T>
MemPool<T>::MemPool(unsigned int count) : m_size((int)sizeof(T)), m_count(count) {}

template<class T>
MemPool<T>::~MemPool()
{
    if (!m_blocks.empty())
    {
        for (std::vector<void*>::iterator it = m_blocks.begin();
             it != m_blocks.end(); ++it)
            ::operator delete(*it);
        m_blocks.clear();
    }
}

template<class T>
void* MemPool<T>::alloc()
{
    if (m_size != (int)sizeof(T))
        return ::operator new(sizeof(T));
    void* result = headOfFreeList_;
    if (result != 0)
    {
        headOfFreeList_ = ((MemPoolFreeLink<T>*)result)->next;
    }
    else
    {
        MemPoolFreeLink<T>* arr = (MemPoolFreeLink<T>*)::operator new(m_count * m_size);
        for (unsigned int i = 0; i < m_count - 1U; i++)
        {
            arr[i].next = (void*)&arr[i + 1];
        }
        arr[m_count - 1U].next = 0;
        result = arr;
        headOfFreeList_ = (void*)((char*)arr + sizeof(T));
        m_blocks.push_back((void*)arr);
        DNF_LOG_SCOPE_LINE(0x7d, "./log/Mempool", "class size(%d) cnt(%d)", m_size,
            m_count * (int)m_blocks.size());
    }
    return result;
}

template<class T>
void MemPool<T>::free(void* ptr, unsigned int size)
{
    if (ptr == 0)
        return;
    if ((unsigned int)m_size != size)
    {
        ::operator delete(ptr);
        return;
    }
    MemPoolFreeLink<T>* t = (MemPoolFreeLink<T>*)ptr;
    t->next = headOfFreeList_;
    headOfFreeList_ = t;
    return;
}

template<class T>
void MemPool<T>::free(void* ptr)
{
    if (ptr == 0)
        return;
    MemPoolFreeLink<T>* t = (MemPoolFreeLink<T>*)ptr;
    t->next = headOfFreeList_;
    headOfFreeList_ = t;
    return;
}

MemPool<CUdpRecvBuffer> g_udpRecvPool(10000);
MemPool<CTcpRecvBuffer> g_tcpRecvPool(1000);
MemPool<CTcpSendBuffer> g_tcpSendPool(1000);
MemPool<CPacketBuffer> g_packetBufferPool(1000);
MemPool<CPeer> g_peerPool(1000);
MemPool<CDNFProhibitUser> g_prohibitUserPool(10000);

template class MemPool<CUdpRecvBuffer>;
template class MemPool<CTcpRecvBuffer>;
template class MemPool<CTcpSendBuffer>;
template class MemPool<CPacketBuffer>;
template class MemPool<CPeer>;
template class MemPool<CDNFProhibitUser>;

void* CUdpRecvBuffer::operator new(unsigned int size) { return g_udpRecvPool.alloc(); }
void CUdpRecvBuffer::operator delete(void* ptr) { g_udpRecvPool.free(ptr); }
void CUdpRecvBuffer::operator delete(void* ptr, unsigned int size) { g_udpRecvPool.free(ptr, size); }

void* CTcpRecvBuffer::operator new(unsigned int size) { return g_tcpRecvPool.alloc(); }
void CTcpRecvBuffer::operator delete(void* ptr) { g_tcpRecvPool.free(ptr); }
void CTcpRecvBuffer::operator delete(void* ptr, unsigned int size) { g_tcpRecvPool.free(ptr, size); }

void* CTcpSendBuffer::operator new(unsigned int size) { return g_tcpSendPool.alloc(); }
void CTcpSendBuffer::operator delete(void* ptr) { g_tcpSendPool.free(ptr); }
void CTcpSendBuffer::operator delete(void* ptr, unsigned int size) { g_tcpSendPool.free(ptr, size); }

void* CPacketBuffer::operator new(unsigned int size) { return g_packetBufferPool.alloc(); }
void CPacketBuffer::operator delete(void* ptr) { g_packetBufferPool.free(ptr); }
void CPacketBuffer::operator delete(void* ptr, unsigned int size) { g_packetBufferPool.free(ptr, size); }

// ============================================================
// IQueue / CSwapQueue
// ============================================================
template<class T>
IQueue<T>& IQueue<T>::Get()
{
    static IQueue instance;
    return instance;
}

template<class T>
bool IQueue<T>::SwitchQueue()
{
    if (m_recv->empty())
        return 0;
    T* tmp = m_recv;
    m_recv = m_parse;
    m_parse = tmp;
    return 1;
}

template<class T, int N>
void CSwapQueue<T, N>::SwapQ()
{
    int t = m_recvIdx;
    m_recvIdx = m_parseIdx;
    m_parseIdx = t;
}

template class IQueue<TcpRecvQueue>;
template class IQueue<UdpRecvQueue>;
template class CSwapQueue<TcpRecvQueue, 2>;
template class CSwapQueue<UdpRecvQueue, 2>;
