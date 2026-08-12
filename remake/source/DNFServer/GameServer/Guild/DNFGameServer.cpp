// df_guild_r — DNFGameServer (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFGameServer.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
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

CGameServer::CGameServer()
    : CServerInterface()
{
    m_field10 = 0;
}

CGameServer::CGameServer(stServerInfo* info)
    : CServerInterface(info)
{
    m_field10 = 0;
}

CGameServer::~CGameServer()
{
}

bool CGameServer::Initialize()
{
    if (!CServerInterface::Initialize())
    {
        return false;
    }
    return true;
}

bool CGameServer::Destroy()
{
    if (!CServerInterface::Destroy())
    {
        return false;
    }
    return true;
}

void CGameServer::SetSocket(unsigned int sock)
{
    m_field10 = sock;
}

int CGameServer::GetSocket()
{
    return m_field10;
}

CTcpGameServer::CTcpGameServer()
{
    m_group = 0;
    m_net = 0;
    m_channel = 0;
}

CTcpGameServer::~CTcpGameServer()
{
    m_group = 0;
    m_net = 0;
}

void CTcpGameServer::Init(unsigned int group, CTcpNetSystem* net)
{
    m_group = group;
    m_net = net;
}

void CTcpGameServer::SendToGameServer(char* buf)
{
    m_net->PushTcpSendPacketQ(buf);
}

bool CTcpGameServer::IsValidServer()
{
    return m_group != 0 && m_net != 0;
}

char* CTcpGameServer::makePacketHeader(unsigned short id, unsigned short size)
{
    if (m_net != 0)
    {
        void* p = m_net->Acquire_TcpSendBuffer();
        PacketHeader* pkt = (PacketHeader*)p;
        pkt->packetId = id;
        pkt->packetSize = size;
        pkt->reversed2 = m_group;
        return (char*)pkt;
    }
    return 0;
}

void CTcpGameServer::SetChannelNo(unsigned char channel)
{
    m_channel = channel;
}

unsigned char CTcpGameServer::GetChannelNo()
{
    return (unsigned char)m_channel;
}
