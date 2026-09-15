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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GUI-NEAR-0010 已消除）
void CFrameCountHandler::InitFrameCountInfo(CApplication* app, unsigned int value,
                                            unsigned short tick)
{
    if (value != 0)
    {
        m_app = app;
        memset(&m_bInit, 0, 0x28);
        m_frameCount = value;
        m_fpsInterval = 100 / value;
    }
    else
    {
        throw CDNFException("CFrameCountHandler::InitFrameCountInfo() Exception Break!");
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GUI-DIFF-0103 已消除）
CFrameCountHandler* CFrameCountHandler::GetFrameCountInfo()
{
    unsigned int l1 = 0;
    unsigned int l2 = 0;
    unsigned int l3 = 0;
    unsigned int q;
    unsigned int l4 = 0;
    (void)l1; (void)l2; (void)l3; (void)q; (void)l4;
    struct tms t;
    m_state = 0;
    if (m_bInit == 0)
    {
        m_bInit = 1;
        m_secondFrameCnt = 0;
        m_startClock = times(&t);
        while ((int)m_startClock == -1)
        {
            throw CDNFException(
                "CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
    }
    else
    {
        m_curClock = times(&t);
        while ((int)m_curClock == -1)
        {
            throw CDNFException(
                "CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
        if (m_startClock > m_curClock)
        {
            m_startClock = m_curClock;
        }
        l1 = m_curClock - m_startClock;
        q = l1 / m_fpsInterval;
        if (m_secondFrameCnt < q)
        {
            m_secondFrameCnt++;
            m_state = 1;
            l2 = m_curClock - m_startClock;
            if (99 < l2)
            {
                m_fps = m_secondFrameCnt;
                m_state = 2;
                m_secondFrameCnt = 0;
                m_startClock = m_curClock - l2 + 100;
                m_field20 = 0;
                if (0x3b < (unsigned char)(++m_secCnt))
                {
                    m_state = 3;
                    m_secCnt = 0;
                    if (0x3b < (unsigned char)(++m_minCnt))
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
