// df_dbmw_r - DNFServerHandler (ORIG DNFServerHandler.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

CServerHandler::CServerHandler()
{
    m_app = 0;
    m_tickCount = 0;
}
CServerHandler::~CServerHandler()
{
    for (std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        CTcpServer* server = it->second;
        if (server)
        {
            delete server;
            it->second = 0;
        }
    }
    m_tcpServers.clear();
}
void CServerHandler::Load(ST_ServerInfo* infos)
{
    for (int i = 0; i <= 0xfe; i++)
    {
        ST_ServerInfo& info = infos[i];
        if (info.m_type == 1)
        {
            unsigned char idx = info.m_idx;
            if (idx == 0xff)
                throw CDNFException("CGameServerHandler::Load() Server Table Exception Break!");
            m_gameServers[idx].Init(info.m_flag, info.m_name, info.m_port, idx);
        }
        if (info.m_type == 3)
        {
            unsigned char idx = info.m_idx;
            if (idx == 0xff || idx != 0xc9)
                throw CDNFException("CServerHandler::Load() Monitor Server Table Exception Break!");
            m_monitorServer.Init(info.m_flag, info.m_name, info.m_port, 0xc9);
        }
        if (info.m_type == 5)
        {
            unsigned char idx = info.m_idx;
            if (idx == 0xff || idx != 0xcb)
                throw CDNFException("CServerHandler::Load() Guild Server Table Exception Break!");
            m_guildServer.Init(info.m_flag, info.m_name, info.m_port, 0xcb);
        }
        if (info.m_type == 7)
        {
            unsigned char idx = info.m_idx;
            if (idx != 0xcd)
                throw CDNFException("CServerHandler::Load() Statistics Server Table Exception Break!");
            m_statisticsServer.Init(info.m_flag, info.m_name, info.m_port, 0xcd);
        }
    }
}
void CServerHandler::Process()
{
    if (m_tickCount++ > 3)
    {
        m_monitorServer.SendHeartBeat();
        m_guildServer.SendHeartBeat();
        m_statisticsServer.SendHeartBeat();
        m_tickCount = 0;
    }
    CheckTcpServerHeartbeat();
}
void CServerHandler::Attach(CApplication* app)
{
    if (app)
        m_app = app;
}
CGameServer* CServerHandler::GetGameServer(int idx)
{
    if (idx <= 0xfe && m_gameServers[idx].IsValidGameServer())
        return &m_gameServers[idx];
    CMyFileLog log("GetGameServer", 0xec);
    log("./log/GameServer.log", "Game Server Index Over Index : %d!\n", idx);
    return 0;
}
void CServerHandler::SendAllToGameServer(char* buf, int len)
{
    CGameServer* p = m_gameServers;
    for (int i = 0xff; i != 0; i--, p++)
        p->SendToServer(buf, len);
}
char CServerHandler::CreateTcpServer(unsigned char idx, unsigned int port)
{
    CTcpServer* server = new CTcpServer;
    server->Init(port, m_app->Get_TcpNetSystem());
    server->SetServerType(idx);
    if (m_tcpServers.insert(std::make_pair(idx, server)).second)
        return 1;
    delete server;
    return 0;
}
char CServerHandler::DeleteTcpServer(unsigned char idx)
{
    std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.find(idx);
    if (it != m_tcpServers.end())
    {
        CTcpServer* server = it->second;
        if (server)
            delete server;
        m_tcpServers.erase(it);
        CMyFileLog log("DeleteTcpServer", 0x130);
        log("./log/TcpServer", "TcpServer(%d) Deleted", idx);
        return 1;
    }
    return 0;
}
CTcpServer* CServerHandler::GetTcpServer(unsigned char idx)
{
    std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.find(idx);
    if (it != m_tcpServers.end())
        return it->second;
    return 0;
}
CTcpServer* CServerHandler::GetTcpServer(unsigned int socket)
{
    for (std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        CTcpServer* server = it->second;
        if ((unsigned int)server->GetSocket() == socket)
            return server;
    }
    return 0;
}
void CServerHandler::SendAllTcpServer(PacketHeader* header)
{
    for (std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        CTcpServer* server = it->second;
        if (server->IsValidServer())
        {
            char* buf = (char*)server->makePacketHeader(header->packetId, header->packetSize);
            memcpy(buf + 0xa, (char*)header + 0xa, header->packetSize - 0xa);
            server->SendToServer(buf);
        }
    }
}
void CServerHandler::CheckTcpServerHeartbeat()
{
    for (std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        CTcpServer* server = it->second;
        if (server && server->IsHeartbeatTimeOver())
        {
            CPeer* peer = m_app->Get_TcpNetSystem()->GetPeer((unsigned int)server->GetSocket());
            if (peer)
            {
                peer->DisConnSig();
                m_app->Get_TcpNetSystem()->DeletePeer(peer);
            }
            m_tcpServers.erase(it);
            break;
        }
    }
}
CGuildServer* CServerHandler::GetGuildServer() { return &m_guildServer; }
CMonitorServer* CServerHandler::GetMonitorServer() { return &m_monitorServer; }
CStatisticsServer* CServerHandler::GetStatisticsServerPtr() { return &m_statisticsServer; }
int CServerHandler::GetAlivedMonitorServer() { return m_tcpServers.size(); }
void CServerHandler::ResetHeartBeat(unsigned char idx) {}
char CServerHandler::IsConnectedMonitorServer(unsigned char idx) { return 0; }
void CServerHandler::SetConnectFlag(unsigned char idx, bool flag) {}
void CServerHandler::SendToTcpServer(PacketHeader* header, unsigned char idx)
{
    CTcpServer* server = GetTcpServer(idx);
    if (!server)
        return;
    char* buf = (char*)server->makePacketHeader(header->packetId, header->packetSize);
    memcpy(buf + 0xa, (char*)header + 0xa, header->packetSize - 0xa);
    server->SendToServer(buf);
}
void CServerHandler::SendToTcpServer(char* buf, int len, unsigned char idx)
{
    CTcpServer* server = GetTcpServer(idx);
    if (!server)
        return;
    server->SendToServer(buf);
}
void CServerHandler::SendAllToMonitorServer(char* buf, int len)
{
    m_monitorServer.SendToServer(buf, len);
}
