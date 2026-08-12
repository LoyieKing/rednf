// df_guild_r — DNFMonitorServer (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFMonitorServer.h"
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

CMonitorServer::CMonitorServer()
    : CServerInterface()
{
}

CMonitorServer::CMonitorServer(stServerInfo* info)
    : CServerInterface(info)
{
}

CMonitorServer::~CMonitorServer()
{
}

bool CMonitorServer::Initialize()
{
    if (!CServerInterface::Initialize())
    {
        return false;
    }
    return true;
}

bool CMonitorServer::Destroy()
{
    if (!CServerInterface::Destroy())
    {
        return false;
    }
    return true;
}

void CMonitorServer::SendHeartBeat(int group)
{
    if (GetUdpHandler() != 0)
    {
        Packet_Monitor_UDP_HeartBeat pkt;
        pkt.m_fieldA = (unsigned char)group;
        ((CUdpHandler*)GetUdpHandler())->SendToServer((char*)&pkt, 0xb,
            GetServerInfo()->m_port, GetServerInfo()->m_name);
    }
}
