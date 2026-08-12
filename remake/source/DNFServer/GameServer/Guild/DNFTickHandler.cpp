// df_guild_r — DNFTickHandler (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFTickHandler.h"
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
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
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

CFrameCountHandler::CFrameCountHandler()
{
    m_field28 = 0;
    m_field2c = 0;
}

void CFrameCountHandler::InitFrameCountInfo(CApplication* app, unsigned int value,
                                            unsigned short tick)
{
    unsigned short t = tick;
    if (value != 0)
    {
        m_field2c = (int)app;
        memset(this, 0, 0x28);
        m_field4 = (int)value;
        m_field8 = 100 / value;
    }
    else
    {
        throw CDNFException("CFrameCountHandler::InitFrameCountInfo() Exception Break!");
    }
}

CFrameCountHandler* CFrameCountHandler::GetFrameCountInfo()
{
    struct tms t;
    *(unsigned char*)((char*)this + 0x24) = 0;
    if (*(unsigned char*)this == 0)
    {
        *(unsigned char*)this = 1;
        m_field14 = 0;
        m_fieldc = (int)times(&t);
        if (m_fieldc == -1)
        {
            throw CDNFException(
                "CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
    }
    else
    {
        m_field10 = (int)times(&t);
        if (m_field10 == -1)
        {
            throw CDNFException(
                "CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
        if ((unsigned int)m_fieldc > (unsigned int)m_field10)
        {
            m_fieldc = m_field10;
        }
        if ((unsigned int)m_field14 <
            (unsigned int)(m_field10 - m_fieldc) / (unsigned int)m_field8)
        {
            m_field14 = m_field14 + 1;
            *(unsigned char*)((char*)this + 0x24) = 1;
            if (99 < (unsigned int)(m_field10 - m_fieldc))
            {
                m_field18 = m_field14;
                *(unsigned char*)((char*)this + 0x24) = 2;
                m_field14 = 0;
                m_fieldc = m_field10 - (m_field10 - m_fieldc) + 100;
                m_field20 = 0;
                *(unsigned char*)((char*)this + 0x25) =
                    (unsigned char)(*(unsigned char*)((char*)this + 0x25) + 1);
                if (0x3b < *(unsigned char*)((char*)this + 0x25))
                {
                    *(unsigned char*)((char*)this + 0x24) = 3;
                    *(unsigned char*)((char*)this + 0x25) = 0;
                    *(unsigned char*)((char*)this + 0x26) =
                        (unsigned char)(*(unsigned char*)((char*)this + 0x26) + 1);
                    if (0x3b < *(unsigned char*)((char*)this + 0x26))
                    {
                        *(unsigned char*)((char*)this + 0x24) = 4;
                        *(unsigned char*)((char*)this + 0x26) = 0;
                    }
                }
            }
        }
    }
    return this;
}

void CFrameCountHandler::SaveProcess()
{
    m_field28 = (char)(m_field28 + 1);
    if (m_field28 != 0)
    {
        DNF_LOG_SCOPE_LINE(0xa8, "./log/frame", "FPS(%02d) / DFC(%02d)\n", m_field18, m_field4);
        m_field28 = 0;
    }
}

void CFrameCountHandler::SaveProcess(int interval)
{
    m_field28 = (char)(m_field28 + 1);
    if (m_field28 != 0)
    {
        DNF_LOG_SCOPE_LINE(0xb8, "./log/frame", "Thread(%2d) / FPS(%02d) / DFC(%02d)", interval, m_field18, m_field4);
        m_field28 = 0;
    }
}
