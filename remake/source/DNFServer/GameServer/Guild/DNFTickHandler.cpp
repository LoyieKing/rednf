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
    m_logCnt = 0;
    m_app = 0;
}

void CFrameCountHandler::InitFrameCountInfo(CApplication* app, unsigned int value,
                                            unsigned short tick)
{
    if (value != 0)
    {
        m_app = (int)app;
        memset(&m_bInit, 0, 0x28);
        m_frameCount = value;
        m_fpsInterval = 100 / value;
    }
    else
    {
        throw CDNFException("CFrameCountHandler::InitFrameCountInfo() Exception Break!");
    }
}

CFrameCountHandler* CFrameCountHandler::GetFrameCountInfo()
{
    struct tms t;
    int u0 = 0, u1 = 0, u2 = 0, u3 = 0;
    (void)u0; (void)u1; (void)u2; (void)u3;
    m_state = 0;
    if (m_bInit == 0)
    {
        m_bInit = 1;
        m_secondFrameCnt = 0;
        m_startClock = (int)times(&t);
        if (m_startClock == -1)
        {
            throw CDNFException(
                "CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
    }
    else
    {
        m_curClock = (int)times(&t);
        if (m_curClock == -1)
        {
            throw CDNFException(
                "CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
        if ((unsigned int)m_startClock > (unsigned int)m_curClock)
        {
            m_startClock = m_curClock;
        }
        if ((unsigned int)m_secondFrameCnt <
            (unsigned int)(m_curClock - m_startClock) / (unsigned int)m_fpsInterval)
        {
            m_secondFrameCnt = m_secondFrameCnt + 1;
            m_state = 1;
            if (99 < (unsigned int)(m_curClock - m_startClock))
            {
                m_fps = m_secondFrameCnt;
                m_state = 2;
                m_secondFrameCnt = 0;
                m_startClock = m_curClock - (m_curClock - m_startClock) + 100;
                m_field20 = 0;
                m_secCnt = (unsigned char)(m_secCnt + 1);
                if (0x3b < m_secCnt)
                {
                    m_state = 3;
                    m_secCnt = 0;
                    m_minCnt = (unsigned char)(m_minCnt + 1);
                    if (0x3b < m_minCnt)
                    {
                        m_state = 4;
                        m_minCnt = 0;
                    }
                }
            }
        }
    }
    return this;
}

void CFrameCountHandler::SaveProcess()
{
    ++m_logCnt;
    register bool b asm("al") = m_logCnt != 0;
    if (b)
    {
        DNF_LOG_SCOPE_LINE(0xa8, "./log/frame", "FPS(%02d) / DFC(%02d)\n", m_fps, m_frameCount);
        m_logCnt = 0;
    }
}

void CFrameCountHandler::SaveProcess(int interval)
{
    ++m_logCnt;
    register bool b asm("al") = m_logCnt != 0;
    if (b)
    {
        DNF_LOG_SCOPE_LINE(0xb8, "./log/frame", "Thread(%2d) / FPS(%02d) / DFC(%02d)", interval, m_fps, m_frameCount);
        m_logCnt = 0;
    }
}
