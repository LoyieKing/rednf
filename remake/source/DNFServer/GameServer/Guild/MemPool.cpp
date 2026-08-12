// df_guild_r — MemPool (split from source/guild per ORIG layout)
#include <stdio.h>
#include <utility>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "MemPool.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

template<class T>
struct MemPoolFreeLink
{
    char pad[sizeof(T) - 4];
    void* next;
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

template class MemPool<CUdpRecvBuffer>;

template class MemPool<CBlackUser>;

template class MemPool<CCashObject>;

template class MemPool<CPacketBuffer>;

template class MemPool<CTcpRecvBuffer>;

template class MemPool<CTcpSendBuffer>;

template class MemPool<STPowerWarGuildInfo>;

template class MemPool<STPowerWarCharacInfo>;

template class MemPool<CPeer>;

template class MemPool<CUser>;

template class MemPool<CGuild>;
