// df_manager_r — CUdpRecvBuffer/CTcpRecvBuffer/CTcpSendBuffer/CPacketBuffer + MemPool/IQueue/CSwapQueue
//               （ORIG DNFPacketBuffer.cpp）
#include "DNFPacketBuffer.h"

#include "DNFFileLog.h"
#include "Peer.h"
#include "ProhibitUser.h"

template<class T> void* MemPool<T>::headOfFreeList_;

template<class T>
MemPool<T>::MemPool() {}

template<class T>
MemPool<T>::MemPool(unsigned int count) : m_size((int)sizeof(T)), m_count((int)count) {}

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
    void* result;
    if (m_size == (int)sizeof(T))
    {
        void* head = headOfFreeList_;
        if (head == 0)
        {
            void* block = ::operator new((unsigned int)m_size * (unsigned int)m_count);
            for (unsigned int i = 0; i < (unsigned int)m_count - 1; i++)
            {
                *(void**)((char*)block + i * m_size + m_size - 4) =
                    (void*)((i + 1) * m_size + (unsigned int)block);
            }
            *(void**)((char*)block + ((unsigned int)m_count - 1) * m_size + m_size - 4) = 0;
            headOfFreeList_ = (void*)((char*)block + m_size);
            head = block;
            m_blocks.push_back((void*)block);
            CMyFileLog log("alloc", 0x7d);
            log("./log/Mempool", "class size(%d) cnt(%d)", m_size,
                m_count * (int)m_blocks.size());
        }
        else
        {
            headOfFreeList_ = *(void**)((char*)head + m_size - 4);
        }
        result = head;
    }
    else
    {
        result = ::operator new(sizeof(T));
    }
    return result;
}

template<class T>
void MemPool<T>::free(void* ptr, unsigned int size)
{
    if (ptr != 0)
    {
        if ((unsigned int)m_size == size)
        {
            *(void**)((char*)ptr + m_size - 4) = headOfFreeList_;
            headOfFreeList_ = ptr;
        }
        else
        {
            ::operator delete(ptr);
        }
    }
}

template<class T>
void MemPool<T>::free(void* ptr)
{
    if (ptr != 0)
    {
        *(void**)((char*)ptr + m_size - 4) = headOfFreeList_;
        headOfFreeList_ = ptr;
    }
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
char IQueue<T>::SwitchQueue()
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
