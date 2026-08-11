// df_dbmw_r - DNFPacketBuffer (ORIG DNFPacketBuffer.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

// MemPool 空闲链表指针存放在对象尾部（sizeof(T)-4）。用成员访问形态让编译器把
// 常量偏移折叠进寻址（ORIG 为 mov %edx,0x1800(%eax) 形态，纯指针运算会产生 lea）。
template<class T>
struct MemPoolFreeLink
{
    char pad[sizeof(T) - 4];
    void* next;
};

CSwapQueue<TcpRecvQueue, 2>* CTcpNetSystem::Get_TcpSwapQPacket() { return &m_recvSwapQueue; }
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
        for (std::vector<void*>::iterator it = m_blocks.begin(); it != m_blocks.end(); ++it)
            ::operator delete(*it);
        m_blocks.clear();
    }
}
template<class T>
void* MemPool<T>::alloc()
{
    if (m_size != (int)sizeof(T))
        return ::operator new(sizeof(T));
    void* head = headOfFreeList_;
    if (head != 0)
    {
        headOfFreeList_ = *(void**)((char*)head + m_size - 4);
    }
    else
    {
        T* block = (T*)::operator new((unsigned int)m_size * (unsigned int)m_count);
        for (unsigned int i = 0; i < (unsigned int)m_count - 1; i++)
        {
            ((MemPoolFreeLink<T>*)((char*)block + i * sizeof(T)))->next =
                (void*)((i + 1) * sizeof(T) + (unsigned int)block);
        }
        ((MemPoolFreeLink<T>*)((char*)block + ((unsigned int)m_count - 1) * sizeof(T)))->next = 0;
        headOfFreeList_ = (void*)((char*)block + sizeof(T));
        head = block;
        m_blocks.push_back((void*)block);
        CMyFileLog log(__FUNCTION__, 0x7d);
        log("./log/Mempool", "class size(%d) cnt(%d)", m_size,
            m_count * (int)m_blocks.size());
    }
    return head;
}
template<class T>
void MemPool<T>::free(void* ptr, unsigned int size)
{
    if (ptr == 0) return;
    if ((unsigned int)m_size != size)
    {
        ::operator delete(ptr);
    }
    else
    {
        void* p = ptr;
        ((MemPoolFreeLink<T>*)((char*)p))->next = headOfFreeList_;
        headOfFreeList_ = p;
    }
}
template<class T>
void MemPool<T>::free(void* ptr)
{
    if (ptr == 0) return;
    void* p = ptr;
    ((MemPoolFreeLink<T>*)((char*)p))->next = headOfFreeList_;
    headOfFreeList_ = p;
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
