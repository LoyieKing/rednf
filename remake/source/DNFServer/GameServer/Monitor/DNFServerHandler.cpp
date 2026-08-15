// df_monitor_r — DNFServerHandler（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFServerHandler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFAppConfig.h"
#include "DNFApplication.h"
#include "DNFGameServer.h"
#include "DNFPacketTranslater.h"
#include "DNFServerInterface.h"
#include "DNFTableBase.h"
#include "TcpNetSystem.h"

CServerHandler::CServerHandler()
    : m_dbServer(0), m_managerServer(0), m_app(0), m_managerHeartbeatCnt(0),
      m_dbHeartbeatCnt(0), m_managerTcpHeartbeatCnt(0)
{
}

CServerHandler::~CServerHandler()
{
    if (m_dbServer != 0)
    {
        m_dbServer->Destroy();
        delete m_dbServer;
        m_dbServer = 0;
    }
    if (m_managerServer != 0)
    {
        m_managerServer->Destroy();
        delete m_managerServer;
        m_managerServer = 0;
    }
}

void CServerHandler::Attach(CApplication* app)
{
    if (app != 0)
    {
        m_app = app;
    }
}

unsigned char CServerHandler::GetServerGroupNo()
{
    return m_app->Get_ServerGroup();
}

void CServerHandler::Process()
{
    if (m_managerServer != 0 && m_managerHeartbeatCnt++ > 3)
    {
        m_managerServer->SendHeartBeat((int)(unsigned char)GetServerGroupNo());
        m_managerHeartbeatCnt = 0;
    }
    for (std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.begin();
         it != m_gameServers.end(); ++it)
    {
        CServerInterface* gs = it->second;
        if (!gs->IsValidServer())
        {
            continue;
        }
        if (gs->IsConnected() && gs->IsHeartBeatTimeOver())
        {
            if (gs->GetChannelNo() <= 0xbd)
            {
                m_app->OnGameServerDown((CGameServer*)gs);
            }
            gs->OnDisconnect();
        }
    }
    if (m_dbServer == 0 || !m_dbServer->IsValidServer())
    {
        return;
    }
    {
        if (m_dbServer->IsConnected() && m_dbServer->IsHeartBeatTimeOver())
        {
            m_dbServer->OnDisconnect();
            DNF_LOG_SCOPE_LINE(0xdc, "./log/DBServerErr", "CServerHandler::Process() DB Server Down!\n");
        }
        if (m_tcpManagerServer.IsValidServer() != 1)
        {
            const char* ip = m_tcpManagerServer.GetIP();
            if (*ip == '\0' || m_tcpManagerServer.GetPort() == 0)
            {
            }
            else
            {
                CTcpNetSystem* net = m_app->Get_TcpNetSystem();
                net->OpenTcpService(*m_tcpManagerServer.GetSockRef(), m_tcpManagerServer.GetIP(),
                                    m_tcpManagerServer.GetPort());
                DNF_LOG_SCOPE_LINE(0x124,"./log/TcpServer", "try connect to DBMW(%s, %d)",
                    m_tcpManagerServer.GetIP(), m_tcpManagerServer.GetPort());
            }
        }
        if (9 < m_managerTcpHeartbeatCnt++)
        {
            m_tcpManagerServer.SendHeartbeat(GetServerGroupNo());
            m_managerTcpHeartbeatCnt = 0;
        }
        if (m_tcpDbServer.IsValidServer() != 1)
        {
            const char* ip = m_tcpDbServer.GetIP();
            if (*ip == '\0' || m_tcpDbServer.GetPort() == 0)
            {
            }
            else
            {
                CTcpNetSystem* net = m_app->Get_TcpNetSystem();
                net->OpenTcpService(*m_tcpDbServer.GetSockRef(), m_tcpDbServer.GetIP(), m_tcpDbServer.GetPort());
                DNF_LOG_SCOPE_LINE(0x13d,"./log/TcpServer", "try connect to DBMW(%s, %d)",
                    m_tcpDbServer.GetIP(), m_tcpDbServer.GetPort());
            }
        }
        if (9 < m_dbHeartbeatCnt++)
        {
            m_tcpDbServer.SendHeartbeat();
            m_dbHeartbeatCnt = 0;
        }
    }
}

void CServerHandler::Load(std::multimap<unsigned int, stServerInfo*>* map)
{
    for (std::multimap<unsigned int, stServerInfo*>::iterator it = map->begin();
         it != map->end(); it++)
    {
        stServerInfo* si = it->second;
        if (si->m_type == 1)
        {
            if (si->m_group == 0xff)
            {
                throw CDNFException("CServerHandler::Load() Server Table Exception Break!");
            }
            RegistGameServer(si);
        }
        else if (si->m_type == 2)
        {
            if (si->m_group == 0xff || si->m_group != 0xc8)
            {
                throw CDNFException("CServerHandler::Load() DB Server Table Exception Break!");
            }
            if (m_dbServer != 0)
            {
                UnregistDBServer();
                DNF_LOG_SCOPE_LINE(0x5d, "./log/Config", "DB Config Reload.\n");
            }
            CDBServer* db = new CDBServer(si);
            db->Initialize();
            RegistDBServer(db);
        }
        else if (si->m_type == 4)
        {
            if (si->m_group == 0xff || si->m_group != 0xca)
            {
                throw CDNFException("CServerHandler::Load() Manager Server Table Exception Break!");
            }
            if (m_managerServer != 0)
            {
                UnregistManagerServer();
                DNF_LOG_SCOPE_LINE(0x6f, "./log/Config", "Manager Config Reload.\n");
            }
            CManagerServer* mgr = new CManagerServer(si);
            mgr->Initialize();
            RegistManagerServer(mgr);
        }
    }
}

bool CServerHandler::RegistGameServer(stServerInfo* info)
{
    unsigned int group = (unsigned int)info->m_group;
    std::map<unsigned int, CGameServer*>::iterator found = m_gameServers.find(group);
    if (found != m_gameServers.end())
        return false;
    CGameServer* gs = new CGameServer(info);
    gs->Initialize();
    m_gameServers.insert(std::pair<const unsigned int, CGameServer*>(info->m_group, gs));
    return true;
}

CGameServer* CServerHandler::GetGameServer(unsigned int id)
{
    std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.find(id);
    if (it == m_gameServers.end())
    {
        return 0;
    }
    return it->second;
}

void* CServerHandler::GetTcpGameServer(unsigned int id)
{
    std::map<unsigned int, CTcpGameServer*>::iterator it = m_tcpGameServers.find(id);
    if (it != m_tcpGameServers.end())
    {
        return it->second;
    }
    return 0;
}

CTcpGameServer* CServerHandler::GetTcpGameServerByCh(unsigned char channel)
{
    for (std::map<unsigned int, CTcpGameServer*>::iterator it = m_tcpGameServers.begin();
         it != m_tcpGameServers.end(); ++it)
    {
        CTcpGameServer* tcp = it->second;
        if (tcp != 0)
        {
            if (tcp->GetChannelNo() == channel)
            {
                return tcp;
            }
        }
    }
    return 0;
}

void CServerHandler::queryReloadTowerRank(unsigned int channel)
{
    int i = 0;
    while ((unsigned int)i <= 4)
    {
        Packet_Request_Load_Tower_Full_Rank pkt;
        pkt.m_rankNo = i;
        pkt.m_channel = channel;
        SendToDB(&pkt);
        i++;
    }
}

int CServerHandler::SendToManager(PacketHeader* pkt)
{
    if (m_managerServer)
    {
        return m_managerServer->SendToServer((char*)pkt,
                                             (unsigned int)pkt->packetSize);
    }
    return 0;
}

void CServerHandler::SendDBMWRequestIPCounter(unsigned char flag, unsigned char b)
{
    Packet_Request_IPCounterList pkt;
    pkt.m_type = flag;
    pkt.m_param = b;
    SendToDB(&pkt);
}

unsigned int CServerHandler::getfirstLinkedServer()
{
    if (m_gameServers.empty())
    {
        return 0xa;
    }
    return m_gameServers.begin()->first;
}

int CServerHandler::RegistDBServer(CDBServer* db)
{
    m_dbServer = db;
    return 1;
}

int CServerHandler::UnregistDBServer()
{
    if (m_dbServer != 0)
    {
        delete m_dbServer;
    }
    m_dbServer = 0;  // ORIG：两个 je 同目标（0 置放在 if 块之后）
    return 1;  // ORIG：末尾 mov $0x1,%eax
}

int CServerHandler::RegistManagerServer(CManagerServer* mgr)
{
    m_managerServer = mgr;
    return 1;
}

CTcpGameServer* CServerHandler::CreateTcpGameServer(unsigned int id)
{
    CTcpGameServer* tcp = new CTcpGameServer;
    tcp->Init(id, m_app->Get_TcpNetSystem());
    if (m_tcpGameServers.insert(std::make_pair(id, tcp)).second)
    {
        return tcp;
    }
    delete tcp;
    return 0;
}

int CServerHandler::DeleteTcpGameServer(unsigned int id)
{
    std::map<unsigned int, CTcpGameServer*>::iterator it = m_tcpGameServers.find(id);
    if (it != m_tcpGameServers.end())
    {
        delete it->second;
        m_tcpGameServers.erase(it);
        DNF_LOG_SCOPE_LINE(0x35f, "./log/Tcp", "TcpGameServer Delete !");
        return 1;
    }
    return 0;
}

int CServerHandler::UnregistGameServer(unsigned int channel)
{
    std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.find(channel);
    if (it == m_gameServers.end())
    {
        return 0;
    }
    delete it->second;
    m_gameServers.erase(it);
    DNF_LOG_SCOPE_LINE(0x412, "./log/GameServer",
        "Game server unregist. Channel: %d", channel);
    return 1;
}

int CServerHandler::UnregistManagerServer()
{
    if (m_managerServer != 0)
    {
        delete m_managerServer;
    }
    m_managerServer = 0;  // ORIG：两个 je 同目标
    return 1;  // ORIG：末尾 mov $0x1,%eax
}

void CServerHandler::SendAllTcpGameServer(PacketHeader* pkt)
{
    CTcpGameServer* tcp = 0;
    char* buf = 0;
    for (std::map<unsigned int, CTcpGameServer*>::iterator it = m_tcpGameServers.begin();
         it != m_tcpGameServers.end(); ++it)
    {
        tcp = it->second;
        if (tcp->IsValidServer() != 0)
        {
            buf = tcp->makePacketHeader(pkt->packetId, pkt->packetSize);
            memcpy(buf + 10, (char*)pkt + 10, (unsigned int)pkt->packetSize - 10U);
            tcp->SendToGameServer(buf);
        }
    }
    __asm__ __volatile__("nop");
}

int CServerHandler::SendAllTcpGameServer(PacketHeader* pkt, int channel)
{
    int count = 0;
    CTcpGameServer* tcp = 0;
    char* buf = 0;
    for (std::map<unsigned int, CTcpGameServer*>::iterator it = m_tcpGameServers.begin();
         it != m_tcpGameServers.end(); ++it)
    {
        tcp = it->second;
        if (tcp->IsValidServer() && tcp->GetChannelType() == channel)
        {
            buf = tcp->makePacketHeader(pkt->packetId, pkt->packetSize);
            memcpy(buf + 10, (char*)pkt + 10, (unsigned int)pkt->packetSize - 10U);
            tcp->SendToGameServer(buf);
            count++;
        }
    }
    return count;
}

void CServerHandler::SendAllToGameServer(char* buf, int len)
{
    for (std::map<unsigned int, CGameServer*>::iterator it = m_gameServers.begin();
         it != m_gameServers.end(); it++)
    {
        if (it->second->IsValidServer())
        {
            it->second->SendToServer(buf, len);
        }
    }
}

void CServerHandler::SendToGameServer(unsigned char channel, PacketHeader* pkt)
{
    CGameServer* gs = GetGameServer((unsigned int)channel);
    if (gs != 0)
    {
        ((CServerInterface*)gs)->SendToServer((char*)pkt, pkt->packetSize);
    }
}

void CServerHandler::SetManagerConnectFlag(bool flag)
{
    if (m_managerServer != 0)
    {
        ((CServerInterface*)m_managerServer)->SetConnFlag(flag);
    }
}

void CServerHandler::SetDBConnectFlag(bool flag)
{
    if (m_dbServer != 0)
    {
        ((CServerInterface*)m_dbServer)->SetConnFlag(flag);
    }
}

void CServerHandler::ResetDBHeartBeat()
{
    if (m_dbServer != 0)
    {
        ((CServerInterface*)m_dbServer)->ResetHeartBeat();
    }
}

bool CServerHandler::IsConnectedDBServer()
{
    if (m_dbServer != 0)
    {
        return m_dbServer->IsConnected();
    }
    return 0;
}

void CServerHandler::SendDBMWConnectionCheck()
{
    Packet_DBMW_Connection_Check pkt;
    pkt.m_channel = 0xc9;
    SendToDB(&pkt);
}

void CServerHandler::ResetHeartBeat(unsigned char channel)
{
    std::map<unsigned int, CGameServer*>::iterator it =
        m_gameServers.find((unsigned int)channel);
    if (it != m_gameServers.end())
    {
        CServerInterface* gs = (CServerInterface*)it->second;
        if (channel != 0xff && gs->IsValidServer() != 0)
        {
            gs->ResetHeartBeat();
            return;
        }
    }
    DNF_LOG_SCOPE_LINE(0x162, "./log/GameServer",
        "CServerHandler::ResetHeartBeat\tGame Server Index Over Index : %d!\n", channel);
}

bool CServerHandler::IsConnectedGameServer(unsigned char channel)
{
    std::map<unsigned int, CGameServer*>::iterator it =
        m_gameServers.find((unsigned int)channel);
    if (it != m_gameServers.end())
    {
        return ((CServerInterface*)it->second)->IsConnected();
    }
    DNF_LOG_SCOPE_LINE(0x19e, "./log/GameServer",
        "CServerHandler::IsConnectedGameServer\tGame Server Index Over Index : %d!\n",
        channel);
    return 0;
}

void CServerHandler::SetConnectFlag(unsigned char channel, bool flag)
{
    std::map<unsigned int, CGameServer*>::iterator it =
        m_gameServers.find((unsigned int)channel);
    if (it != m_gameServers.end())
    {
        ((CServerInterface*)it->second)->SetConnFlag(flag);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x1f8, "./log/GameServer",
            "CServerHandler::SetConnectFlag\tGame Server Index Over Index : %d!\n",
            channel);
    }
}

void CServerHandler::SendDBMWRequestARSInfo(unsigned char flag)
{
    Packet_Web_Request_ARS_Info pkt;
    pkt.m_flag = flag;
    SendToDB(&pkt);
}

CTcpManagerServer* CServerHandler::GetTcpManagerServer() { return &m_tcpManagerServer; }

CTcpDBServer* CServerHandler::GetTcpDBServer() { return &m_tcpDbServer; }

void CServerHandler::SendToDB(PacketHeader* pkt)
{
    if (m_dbServer)
    {
        m_dbServer->SendToServer((char*)pkt, pkt->packetSize);
    }
}

CDBServer* CServerHandler::GetDBServer() { return m_dbServer; }

CManagerServer* CServerHandler::GetManagerServer() { return m_managerServer; }

void CServerHandler::SetGameServerIpPort(unsigned char a, unsigned int b, unsigned short c)
{
    (void)a;
    (void)c;
}

void CServerHandler::QueryMember(unsigned int key)
{
    Packet_DB_Query_Member pkt;
    pkt.m_memberKey = key;
    SendToDB(&pkt);
}

void CServerHandler::QueryMemberMember(unsigned int key)
{
    Packet_DB_Query_Member_Member pkt;
    pkt.m_memberKey = key;
    SendToDB(&pkt);
}

void CServerHandler::SendDBMWRequest_D_IPCounter(unsigned char flag)
{
    Packet_Request_IPCounterList pkt;
    pkt.m_type = flag;
    SendToDB(&pkt);
}
