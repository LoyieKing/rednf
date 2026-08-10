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
void* MemPool<T>::headOfFreeList_ = 0;

template<class T>
MemPool<T>::MemPool(unsigned int count)
{
    m_classSize = (int)sizeof(T);
    m_count = count;
}

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
    void* result;
    if (m_classSize == (int)sizeof(T))
    {
        if (headOfFreeList_ == 0)
        {
            void* block = ::operator new(m_classSize * m_count);
            for (unsigned int i = 0; i < m_count - 1U; i++)
            {
                *(void**)((char*)block + i * m_classSize + (m_classSize - 4)) =
                    (void*)((i + 1) * m_classSize + (int)block);
            }
            *(void**)((char*)block + (m_count - 1) * m_classSize + (m_classSize - 4)) = 0;
            headOfFreeList_ = (void*)((char*)block + m_classSize);
            result = block;
            m_chunks.push_back(std::move(block));
            DNF_LOG_SCOPE_LINE(0x7d, "./log/Mempool", "class size(%d) cnt(%d)", m_classSize,
                m_count * (int)m_chunks.size());
        }
        else
        {
            result = headOfFreeList_;
            headOfFreeList_ = *(void**)((char*)headOfFreeList_ + (m_classSize - 4));
        }
    }
    else
    {
        result = ::operator new(m_classSize);
    }
    return result;
}

template<class T>
void MemPool<T>::free(void* p)
{
    if (p != 0)
    {
        *(void**)((char*)p + (sizeof(T) - 4)) = headOfFreeList_;
        headOfFreeList_ = p;
    }
}

template<class T>
void MemPool<T>::free(void* p, unsigned int size)
{
    if (p != 0)
    {
        if (m_classSize == (int)size)
        {
            *(void**)((char*)p + (m_classSize - 4)) = headOfFreeList_;
            headOfFreeList_ = p;
        }
        else
        {
            ::operator delete(p);
        }
    }
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

