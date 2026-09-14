// df_monitor_r — DNFPacketBuffer（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFPacketBuffer.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFBuddy.h"
#include "DNFMember.h"
#include "DNFProhibitUser.h"
#include "DNFUser.h"
#include "Peer.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

template<class T>
void* MemPool<T>::headOfFreeList_;

template<class T>
struct MemPoolSlot
{
    char pad[sizeof(T) - 4];
    void* next;
};

template<class T>
MemPool<T>::MemPool() {}
template<class T>
MemPool<T>::MemPool(unsigned int count) : m_size((unsigned int)sizeof(T)), m_count(count) {}
template<class T>
MemPool<T>::~MemPool()
{
    if (!m_blocks.empty())
    {
        for (std::vector<void*>::iterator it = m_blocks.begin(); it != m_blocks.end(); ++it)
        {
            ::operator delete(*it);
        }
        m_blocks.clear();
    }
}

template<class T>
void* MemPool<T>::alloc()
{
    if (m_size != (int)sizeof(T))
    {
        return ::operator new(sizeof(T));
    }
    void* head = headOfFreeList_;
    if (head != 0)
    {
        headOfFreeList_ = ((MemPoolSlot<T>*)head)->next;
    }
    else
    {
        MemPoolSlot<T>* block = (MemPoolSlot<T>*)::operator new((unsigned int)m_count * (unsigned int)m_size);
        for (unsigned int i = 0; i < (unsigned int)m_count - 1; i++)
        {
            block[i].next = (void*)&block[i + 1];
        }
        block[(unsigned int)m_count - 1].next = 0;
        head = (void*)block;
        headOfFreeList_ = (void*)((char*)block + sizeof(T));
        m_blocks.push_back((void*)block);
        DNF_LOG_SCOPE_LINE(0x7d, "./log/Mempool", "class size(%d) cnt(%d)", m_size,
            m_count * (int)m_blocks.size());
    }
    return head;
}

template<class T>
void MemPool<T>::free(void* ptr, unsigned int size)
{
    if (ptr == 0)
    {
        return;
    }
    if ((unsigned int)m_size != size)
    {
        ::operator delete(ptr);
        return;
    }
    MemPoolSlot<T>* slot = (MemPoolSlot<T>*)ptr;
    slot->next = headOfFreeList_;
    headOfFreeList_ = slot;
}

template<class T>
void MemPool<T>::free(void* ptr)
{
    if (ptr == 0)
    {
        return;
    }
    MemPoolSlot<T>* slot = (MemPoolSlot<T>*)ptr;
    slot->next = headOfFreeList_;
    headOfFreeList_ = slot;
}

template class MemPool<CUdpRecvBuffer>;
template class MemPool<CTcpRecvBuffer>;
template class MemPool<CTcpSendBuffer>;
template class MemPool<CPacketBuffer>;
template class MemPool<CUser>;
template class MemPool<CMember>;
template class MemPool<CCashObject>;
template class MemPool<CBuddy>;
template class MemPool<CBlackUser>;
template class MemPool<CPeer>;
template class MemPool<CDNFProhibitUser>;

int getErrno()
{
    return *__errno_location();
}

// ---- ORIG 静态对象池：11 个池的 (符号, count) 全部来自 ORIG df_monitor_r 的
//      全局构造器 `__static_initialization_and_destruction_0`：
//        CUdpRecvBuffer::m_RecvBufferMemPool_  @0x81927a8 count 0x2710
//        CTcpRecvBuffer::m_BufferMemPool_      @0x81927bc count 0x3e8
//        CTcpSendBuffer::m_BufferMemPool_      @0x81927d0 count 0x3e8
//        CPacketBuffer::m_BufferMemPool_       @0x81927e4 count 0x3e8
//        CPeer::pool_                          count 0x3e8
//        CUser::m_UsermemPool_                 count 0x6d60
//        CMember::m_MembermemPool_             count 0x4e20
//        CBuddy::m_Pool_                       count 0x61a80
//        CBlackUser::m_memPool_                count 0x186a0
//        CCashObject::m_Pool_                  count 0x1388
//        CDNFProhibitUser::m_ProhibitUsermemPool_ count 0x2710
//      对应 ORIG 的 `X::operator new` = `MemPool<X>::alloc()`、
//      `operator delete(p)` = `free(p)`、`operator delete(p,n)` = `free(p,n)`。
//      这一层不做的话，运行时用 ::operator new 而不进池，启动期就不会有
//      log/Mempool 那 4 行（`class size(620608) cnt(1000)` 等），
//      整机 A/B 里 monitor/Mempool 会整份缺失（ORIG 每轮 4 行 / 我们 0 行）。
MemPool<CUdpRecvBuffer> CUdpRecvBuffer::m_RecvBufferMemPool_(0x2710);
MemPool<CTcpRecvBuffer> CTcpRecvBuffer::m_BufferMemPool_(0x3e8);
MemPool<CTcpSendBuffer> CTcpSendBuffer::m_BufferMemPool_(0x3e8);
MemPool<CPacketBuffer> CPacketBuffer::m_BufferMemPool_(0x3e8);

void* CUdpRecvBuffer::operator new(unsigned int size)
{
    (void)size;   // ORIG 忽略入参，池按 sizeof(T) 切分
    return m_RecvBufferMemPool_.alloc();
}

void CUdpRecvBuffer::operator delete(void* ptr)
{
    m_RecvBufferMemPool_.free(ptr);
}

void* CTcpRecvBuffer::operator new(unsigned int size)
{
    (void)size;
    return m_BufferMemPool_.alloc();
}

void CTcpRecvBuffer::operator delete(void* ptr)
{
    m_BufferMemPool_.free(ptr);
}

void* CTcpSendBuffer::operator new(unsigned int size)
{
    (void)size;
    return m_BufferMemPool_.alloc();
}

void CTcpSendBuffer::operator delete(void* ptr)
{
    m_BufferMemPool_.free(ptr);
}

void* CPacketBuffer::operator new(unsigned int size)
{
    (void)size;
    return m_BufferMemPool_.alloc();
}

void CPacketBuffer::operator delete(void* ptr)
{
    m_BufferMemPool_.free(ptr);
}

void CUdpRecvBuffer::operator delete(void* p, unsigned int size) { m_RecvBufferMemPool_.free(p, size); }

void CTcpRecvBuffer::operator delete(void* p, unsigned int size) { m_BufferMemPool_.free(p, size); }

void CTcpSendBuffer::operator delete(void* p, unsigned int size) { m_BufferMemPool_.free(p, size); }

void CPacketBuffer::operator delete(void* p, unsigned int size) { m_BufferMemPool_.free(p, size); }
