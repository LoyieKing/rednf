// df_guild_r — DNFDBServer (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFDBServer.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
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

struct DBServerPacketLayout
{
    PacketHeader hdr;
    char m_field_a;   // +0xa
};

void CTcpDBServer::SetIP(std::string ip)
{
    m_ip = ip;
}

void CTcpDBServer::SetPort(unsigned short port)
{
    m_port = port;
}

int CTcpDBServer::GetSock()
{
    return m_sock;
}

int& CTcpDBServer::GetSockRef()
{
    return m_sock;
}

CDBServer::CDBServer()
    : CServerInterface()
{
}

CDBServer::CDBServer(stServerInfo* info)
    : CServerInterface(info)
{
}

CDBServer::~CDBServer()
{
}

bool CDBServer::Initialize()
{
    if (!CServerInterface::Initialize())
    {
        return false;
    }
    return true;
}

bool CDBServer::Destroy()
{
    if (!CServerInterface::Destroy())
    {
        return false;
    }
    return true;
}

CTcpDBServer::CTcpDBServer()
{
    m_port = 0;
    m_sock = -1;
    m_net = 0;
    m_guildMgr = 0;
    // ORIG：string 成员默认构造后再显式 clear()（无临时对象 + assign）
    m_ip.clear();
}

CTcpDBServer::~CTcpDBServer()
{
    Clear();
}

void CTcpDBServer::Clear()
{
    m_net = 0;
    m_guildMgr = 0;
    m_sock = -1;
    m_port = 0;
    m_ip.clear();
}

void CTcpDBServer::SendLogin()
{
    char* buf = makePacketHeader(0x1068, 0xb);
    char* pkt = buf;
    if (pkt != 0)
    {
        ((DBServerPacketLayout*)pkt)->m_field_a = 9;
        SendToServer(pkt);
    }
}

void CTcpDBServer::SendLogout()
{
    char* buf = makePacketHeader(0x1069, 0xb);
    char* pkt = buf;
    if (pkt != 0)
    {
        ((DBServerPacketLayout*)pkt)->m_field_a = 9;
        SendToServer(pkt);
    }
}

void CTcpDBServer::SendHeartbeat()
{
    char* pkt = makePacketHeader(0x106a, 0xb);
    char* buf = pkt;
    if (buf != 0)
    {
        ((DBServerPacketLayout*)buf)->m_field_a = '\t';
        SendToServer(buf);
    }
}

void CTcpDBServer::Connected()
{
    SendLogin();
    m_guildMgr->CargoUnlock();
    DNF_LOG_SCOPE_LINE(0x121, "./log/GuildCargo", "TCP DBMW(%s,%d) CONNECTED! GUILD CARGO ACTIVE!",
        m_ip.c_str(), (unsigned int)m_port);
}

void CTcpDBServer::DisConnected()
{
    m_sock = -1;
    m_guildMgr->CargoLock();
    DNF_LOG_SCOPE_LINE(0x131, "./log/GuildCargo",
        "TCP DBMW(%s,%d) DISCONNECTED! GUILD CARGO DEACTIVE!",
        m_ip.c_str(), (unsigned int)m_port);
}

void CTcpDBServer::Init(CTcpNetSystem* net, CGuildManager* gm)
{
    m_net = net;
    m_guildMgr = gm;
}

void CTcpDBServer::SendToServer(char* buf)
{
    m_net->PushTcpSendPacketQ(buf);
}

bool CTcpDBServer::IsValidServer()
{
    if (m_sock != -1 && m_net != 0)
    {
        return true;
    }
    DNF_LOG_SCOPE_LINE(0x14f, "./log/TcpServer", "Invalid Tcp Server(%d,%x)", m_sock, m_net);
    return false;
}

char* CTcpDBServer::makePacketHeader(unsigned short id, unsigned short size)
{
    if (m_net != 0)
    {
        void* p = m_net->Acquire_TcpSendBuffer();
        PacketHeader* pkt = (PacketHeader*)p;
        pkt->packetId = id;
        pkt->packetSize = size;
        pkt->reversed2 = m_sock;
        return (char*)pkt;
    }
    return 0;
}

char* CTcpDBServer::GetIP()
{
    return (char*)m_ip.c_str();
}

unsigned short CTcpDBServer::GetPort()
{
    return m_port;
}
