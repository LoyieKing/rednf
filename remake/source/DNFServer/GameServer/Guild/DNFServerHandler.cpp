// df_guild_r — DNFServerHandler (split from source/guild per ORIG layout)
#include <string.h>
#include "DNFFunctionLib.h"
#include <stdio.h>
#include "DNFFileLog.h"

#include "DNFServerHandler.h"
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

CTcpDBServer* CServerHandler::GetTcpDBServer()
{
    return &m_tcpDbServer;
}

CServerHandler::CServerHandler()
{
    m_dbServer = 0;
    m_managerServer = 0;
    m_app = 0;
    m_heartbeat = 0;
    m_field58 = 0;
}

CServerHandler::~CServerHandler()
{
    if (m_dbServer != 0)
    {
        delete m_dbServer;
        m_dbServer = 0;
    }
    if (m_managerServer != 0)
    {
        delete m_managerServer;
        m_managerServer = 0;
    }
}

void CServerHandler::Load(std::multimap<unsigned int, stServerInfo*>* map)
{
    if (map == 0)
    {
        return;
    }
    for (std::multimap<unsigned int, stServerInfo*>::iterator it = map->begin();
         it != map->end(); ++it)
    {
        stServerInfo* info = it->second;
        if (info->m_field2 == 1)
        {
            if (info->m_field1 == 0xff)
            {
                throw CDNFException("CServerHandler::Load() Server Table Exception Break!");
            }
            RegistGameServer(info);
        }
        else if (info->m_field2 == 2)
        {
            if (info->m_field1 == 0xff || info->m_field1 != 0xc8)
            {
                throw CDNFException(
                    "CServerHandler::Load() DB Server Table Exception Break!");
            }
            if (m_dbServer != 0)
            {
                UnregistDBServer();
                DNF_LOG_SCOPE_LINE(0x59, "./log/Config", "DB Config Reload.\n");
            }
            CDBServer* db = new CDBServer(info);
            RegistDBServer(db);
        }
        else if (info->m_field2 == 4)
        {
            if (info->m_field1 == 0xff || info->m_field1 != 0xca)
            {
                throw CDNFException(
                    "CServerHandler::Load() Manager Server Table Exception Break!");
            }
            if (m_managerServer != 0)
            {
                UnregistManagerServer();
                DNF_LOG_SCOPE_LINE(0x6b, "./log/Config", "Manager Config Reload.\n");
            }
            CManagerServer* ms = new CManagerServer(info);
            RegistManagerServer(ms);
        }
    }
}

void CServerHandler::Process()
{
    bool doHb = false;
    if (m_managerServer != 0)
    {
        int old = m_heartbeat;
        m_heartbeat = old + 1;
        doHb = old >= 4;
    }
    if (doHb)
    {
        m_managerServer->SendHeartBeat(GetServerGroupNo() & 0xff);
        m_heartbeat = 0;
    }
    for (std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.begin();
         it != m_gameServers.end(); ++it)
    {
        CGameServer* gs = it->second;
        if (gs->IsValidServer() && gs->IsConnected() && gs->IsHeartBeatTimeOver())
        {
            if (gs->GetChannelNo() < 0xbe)
            {
                m_app->OnGameServerDown(gs);
            }
            gs->OnDisconnect();
        }
    }
    if (m_dbServer != 0 && m_dbServer->IsValidServer())
    {
        if (m_dbServer->IsConnected() && m_dbServer->IsHeartBeatTimeOver())
        {
            m_dbServer->OnDisconnect();
            DNF_LOG_SCOPE_LINE(0xea, "./log/DBServerErr", "CServerHandler::Process() DB Server Down!\n");
        }
    }
    if (m_tcpDbServer.IsValidServer() != 1)
    {
        const char* ip = m_tcpDbServer.GetIP();
        if (*ip != '\0' && m_tcpDbServer.GetPort() != 0)
        {
            int sockRef = 0;
            m_app->Get_TcpNetSystem()->OpenTcpService(
                *m_tcpDbServer.GetSockRef(), ip, m_tcpDbServer.GetPort());
            DNF_LOG_SCOPE_LINE(0x135,"./log/TcpServer", "try connect to DBMW(%s, %d)", ip,
                (unsigned int)m_tcpDbServer.GetPort());
        }
    }
    int old = m_field58;
    m_field58 = old + 1;
    if (old > 3)
    {
        m_tcpDbServer.SendHeartbeat();
        m_field58 = 0;
    }
}

void CServerHandler::ResetHeartBeat(unsigned char type)
{
    std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.find((unsigned int)type);
    if (it != m_gameServers.end())
    {
        CGameServer* gs = it->second;
        if (type != 0xff && gs->IsValidServer())
        {
            gs->ResetHeartBeat();
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x15b, "./log/GameServer",
            "CServerHandler::ResetHeartBeat\tGame Server Index Over Index : %d!\n",
            (unsigned int)type);
    }
}

void CServerHandler::ResetDBHeartBeat()
{
    if (m_dbServer != 0)
    {
        m_dbServer->ResetHeartBeat();
    }
}

void CServerHandler::SetGameServerIpPort(unsigned char group, unsigned int port,
                                         unsigned short tcpPort)
{
}

bool CServerHandler::IsConnectedGameServer(unsigned char group)
{
    std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.find(group);
    if (it != m_gameServers.end())
    {
        return it->second->IsConnected();
    }
    DNF_LOG_SCOPE_LINE(0x1a1,"./log/GameServer",
        "CServerHandler::IsConnectedGameServer\tGame Server Index Over Index : %d!\n",
        (unsigned int)group);
    return 0;
}

bool CServerHandler::IsConnectedDBServer()
{
    if (m_dbServer != 0)
    {
        return m_dbServer->IsConnected();
    }
    return 0;
}

void CServerHandler::SetDBConnectFlag(bool flag)
{
    if (m_dbServer != 0)
    {
        m_dbServer->SetConnFlag(flag);
    }
}

void CServerHandler::SetManagerConnectFlag(bool flag)
{
    if (m_managerServer != 0)
    {
        m_managerServer->SetConnFlag(flag);
    }
}

void CServerHandler::SetConnectFlag(unsigned char type, bool flag)
{
    std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.find((unsigned int)type);
    if (it != m_gameServers.end())
    {
        it->second->SetConnFlag(flag);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x1fa, "./log/GameServer",
            "CServerHandler::SetConnectFlag\tGame Server Index Over Index : %d!\n",
            (unsigned int)type);
    }
}

void CServerHandler::Attach(CApplication* app)
{
    if (app != 0)
    {
        m_app = app;
    }
}

void CServerHandler::SendToGameServer(unsigned char group, PacketHeader* pkt)
{
    CGameServer* gs = GetGameServer((unsigned int)group);
    if (gs != 0)
    {
        gs->SendToServer((char*)pkt, pkt->packetSize);
    }
}

void CServerHandler::SendAllUdpGameServer(char* buf, int len)
{
    for (std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.begin();
         it != m_gameServers.end(); ++it)
    {
        CGameServer* gs = it->second;
        if (gs->IsValidServer())
        {
            gs->SendToServer(buf, len);
        }
    }
}

void CServerHandler::SendAllTcpGameServer(PacketHeader* pkt)
{
    for (std::map<unsigned int, CTcpGameServer*>::iterator it = m_tcpGameServers.begin();
         it != m_tcpGameServers.end(); ++it)
    {
        CTcpGameServer* tgs = it->second;
        if (tgs->IsValidServer())
        {
            char* buf = tgs->makePacketHeader(*(unsigned short*)pkt,
                                              *(unsigned short*)((char*)pkt + 2));
            memcpy(buf + 10, (char*)pkt + 10, *(unsigned short*)((char*)pkt + 2) - 10);
            tgs->SendToGameServer(buf);
        }
    }
}

void CServerHandler::SendTcpGameServerFirst(PacketHeader* pkt)
{
    if (!m_tcpGameServers.empty())
    {
        CTcpGameServer* tgs = m_tcpGameServers.begin()->second;
        if (tgs->IsValidServer())
        {
            char* buf = tgs->makePacketHeader(*(unsigned short*)pkt,
                                              *(unsigned short*)((char*)pkt + 2));
            memcpy(buf + 10, (char*)pkt + 10, *(unsigned short*)((char*)pkt + 2) - 10);
            tgs->SendToGameServer(buf);
        }
    }
}

void CServerHandler::QueryGuildMember(unsigned char group, unsigned int characNo)
{
    Packet_DB_Query_Guild_Member pkt;
    pkt.m_group = group;
    pkt.m_characNo = characNo;
    m_dbServer->SendToServer((char*)&pkt, 0xf);
}

void CServerHandler::QueryGuild(unsigned int group, unsigned int guildId)
{
    Packet_DB_Query_Guild pkt;
    *(unsigned char*)((char*)&pkt + 0xa) = GetServerGroupNo();
    *(unsigned int*)((char*)&pkt + 0xb) = group;
    *(unsigned int*)((char*)&pkt + 0xf) = guildId;
    m_dbServer->SendToServer((char*)&pkt, 0x13);
}

void CServerHandler::SendToDB(PacketHeader* pkt)
{
    m_dbServer->SendToServer((char*)pkt, pkt->packetSize);
}

void CServerHandler::SendToManager(PacketHeader* pkt)
{
    m_managerServer->SendToServer((char*)pkt, pkt->packetSize);
}

unsigned char CServerHandler::GetServerGroupNo()
{
    return m_app->Get_ServerGroup();
}

void CServerHandler::SendDBMWConnectionCheck()
{
    Packet_DBMW_Connection_Check pkt;
    pkt.m_data[0] = 0xcb;
    m_dbServer->SendToServer((char*)&pkt, pkt.packetSize);
}

CTcpGameServer* CServerHandler::CreateTcpGameServer(unsigned int group)
{
    CTcpGameServer* server = new CTcpGameServer();
    server->Init(group, m_app->Get_TcpNetSystem());
    std::pair<std::map<unsigned int, CTcpGameServer*>::iterator, bool> result =
        m_tcpGameServers.insert(std::make_pair(group, server));
    if (!result.second)
    {
        delete server;
        server = 0;
    }
    return server;
}

void CServerHandler::DeleteTcpGameServer(unsigned int group)
{
    std::map<unsigned int, CTcpGameServer*>::iterator it = m_tcpGameServers.find(group);
    if (it == m_tcpGameServers.end())
    {
        return;
    }
    CTcpGameServer* tgs = it->second;
    if (tgs != 0)
    {
        delete tgs;
    }
    m_tcpGameServers.erase(it);
    DNF_LOG_SCOPE_LINE(0x33e, "./log/Tcp", "TcpGameServer unregist. Channel: %d", group);
}

CTcpGameServer* CServerHandler::GetTcpGameServer(unsigned int group)
{
    std::map<unsigned int, CTcpGameServer*>::iterator it = m_tcpGameServers.find(group);
    return it == m_tcpGameServers.end() ? 0 : it->second;
}

void CServerHandler::TcpSendToDB(PacketHeader* pkt)
{
    m_tcpDbServer.SendToServer((char*)pkt);
}

bool CServerHandler::RegistGameServer(stServerInfo* info)
{
    unsigned int group = (unsigned int)(unsigned char)info->m_field1;
    std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.find(group);
    if (it != m_gameServers.end())
    {
        return false;
    }
    CGameServer* gs = new CGameServer(info);
    gs->Initialize();
    m_gameServers.insert(std::make_pair((unsigned int)(unsigned char)info->m_field1, gs));
    return true;
}

void CServerHandler::UnregistGameServer(unsigned int group)
{
    std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.find(group);
    if (it == m_gameServers.end())
    {
        return;
    }
    CGameServer* gs = it->second;
    if (gs != 0)
    {
        delete gs;
    }
    m_gameServers.erase(it);
    DNF_LOG_SCOPE_LINE(0x387, "./log/GameServer", "Game server unregist. Channel: %d", group);
}

CGameServer* CServerHandler::GetGameServer(unsigned int group)
{
    std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.find(group);
    return it == m_gameServers.end() ? 0 : it->second;
}

bool CServerHandler::RegistDBServer(CDBServer* server)
{
    m_dbServer = server;
    return true;
}

bool CServerHandler::UnregistDBServer()
{
    if (m_dbServer != 0)
    {
        delete m_dbServer;
    }
    m_dbServer = 0;
    return true;
}

CDBServer* CServerHandler::GetDBServer()
{
    return m_dbServer;
}

bool CServerHandler::RegistManagerServer(CManagerServer* server)
{
    m_managerServer = server;
    return true;
}

bool CServerHandler::UnregistManagerServer()
{
    if (m_managerServer != 0)
    {
        delete m_managerServer;
    }
    m_managerServer = 0;
    return true;
}

CManagerServer* CServerHandler::GetManagerServer()
{
    return m_managerServer;
}

bool CServerHandler::RegistMonitorServer(CMonitorServer* server)
{
    m_monitorServer = server;
    return true;
}

bool CServerHandler::UnregistMonitorServer()
{
    if (m_monitorServer != 0)
    {
        delete m_monitorServer;
    }
    m_monitorServer = 0;
    return true;
}

CMonitorServer* CServerHandler::GetMonitorServer()
{
    return m_monitorServer;
}

#pragma pack(push,1)
struct Packet_DB_Query_Guild_Member_Layout
{
    char pad0x0[0xb];
    unsigned int mb;
};
#pragma pack(pop)
Packet_DB_Query_Guild_Member::Packet_DB_Query_Guild_Member()
    : PacketHeader(0x402, 0xf)
{
    ((Packet_DB_Query_Guild_Member_Layout*)this)->mb = 0;
}

#pragma pack(push,1)
struct Packet_DB_Query_Guild_Layout
{
    char pad0x0[0xa];
    unsigned char ma;
    unsigned int mb;
    unsigned int mf;
};
#pragma pack(pop)
Packet_DB_Query_Guild::Packet_DB_Query_Guild()
    : PacketHeader(0x404, 0x13)
{
    ((Packet_DB_Query_Guild_Layout*)this)->ma = 255;
    ((Packet_DB_Query_Guild_Layout*)this)->mb = 0;
    ((Packet_DB_Query_Guild_Layout*)this)->mf = 0;
}

Packet_DBMW_Connection_Check::Packet_DBMW_Connection_Check()
    : PacketHeader(0x413, 0xb)
{
    (void)0;
}
