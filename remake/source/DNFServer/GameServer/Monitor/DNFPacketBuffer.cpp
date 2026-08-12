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
        void* block = ::operator new((unsigned int)m_count * (unsigned int)m_size);
        for (unsigned int i = 0; i < (unsigned int)m_count - 1; i++)
        {
            ((MemPoolSlot<T>*)(i * sizeof(T) + (unsigned int)block))->next =
                (void*)((i + 1) * sizeof(T) + (unsigned int)block);
        }
        ((MemPoolSlot<T>*)(((unsigned int)m_count - 1) * sizeof(T) + (unsigned int)block))->next = 0;
        head = block;
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

void* CUdpRecvBuffer::operator new(unsigned int size)
{
    return ::operator new(size);
}

void CUdpRecvBuffer::operator delete(void* ptr)
{
    ::operator delete(ptr);
}

void* CTcpRecvBuffer::operator new(unsigned int size)
{
    return ::operator new(size);
}

void CTcpRecvBuffer::operator delete(void* ptr)
{
    ::operator delete(ptr);
}

void* CTcpSendBuffer::operator new(unsigned int size)
{
    return ::operator new(size);
}

void CTcpSendBuffer::operator delete(void* ptr)
{
    ::operator delete(ptr);
}

void* CPacketBuffer::operator new(unsigned int size)
{
    return ::operator new(size);
}

void CPacketBuffer::operator delete(void* ptr)
{
    ::operator delete(ptr);
}

void CUdpRecvBuffer::operator delete(void* p, unsigned int size) { ::operator delete(p); }

void CTcpRecvBuffer::operator delete(void* p, unsigned int size) { ::operator delete(p); }

void CTcpSendBuffer::operator delete(void* p, unsigned int size) { ::operator delete(p); }

void CPacketBuffer::operator delete(void* p, unsigned int size) { ::operator delete(p); }
