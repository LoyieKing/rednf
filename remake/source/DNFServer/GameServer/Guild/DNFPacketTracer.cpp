// df_guild_r — DNFPacketTracer (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Guild_Change_Power_War_Point.h"

#include "DNFPacketTracer.h"
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

CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}

void CPacketTracer::AddLog(int p1, int p2)
{
    time_t t;
    time(&t);
    tm lt = *localtime(&t);
    char buf[32] = {0};
    sprintf(buf, "(%02d:%02d:%02d/%d/%d)",
            lt.tm_hour, lt.tm_min, lt.tm_sec, p2, p1);
    m_strLog += buf;
    m_nCount++;
}

void CPacketTracer::ResetLog()
{
    ((std::string*)((char*)this + 4))->clear();
}

void CPacketTracer::WriteLog()
{
    if (m_nCount % 0x1e == 0)
    {
        DNF_LOG_SCOPE_LINE(0x2a,"./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n",
            m_strLog.c_str());
        ResetLog();
    }
}

void CPacketTracer::AbsoluteWriteLog()
{
    DNF_LOG_SCOPE_LINE(0x32,"./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n",
        ((std::string*)((char*)this + 4))->c_str());
    ResetLog();
}

CPacketTracer::CPacketTracer() : m_nCount(0)
{
}

CPacketTracer::~CPacketTracer()
{
}
