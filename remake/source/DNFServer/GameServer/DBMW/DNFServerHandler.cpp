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

// ORIG 顺序：显式初始化成员(m_app/m_tickCount)先落值，随后才构造
// m_statisticsServer（GCC 4.4 对 init-list 显式项先于隐式默认构造）。
CServerHandler::CServerHandler() : m_app(0), m_tickCount(0)
{
}
CServerHandler::~CServerHandler()
{
    for (std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        if (it->second != NULL)
        {
            delete it->second;
            it->second = 0;
        }
    }
    m_tcpServers.clear();
}
void CServerHandler::Load(ST_ServerInfo* infos)
{
    for (int i = 0; i <= 0xfe; i++)
    {
        unsigned char idx;
        if (infos[i].m_type == 1)
        {
            idx = infos[i].m_idx;
            if (idx == 0xff)
                throw CDNFException("CGameServerHandler::Load() Server Table Exception Break!");
            m_gameServers[idx].Init(infos[i].m_flag, infos[i].m_name,
                                    infos[i].m_port, idx);
        }
        if (infos[i].m_type == 3)
        {
            idx = infos[i].m_idx;
            if (idx == 0xff || idx != 0xc9)
                throw CDNFException("CServerHandler::Load() Monitor Server Table Exception Break!");
            m_monitorServer.Init(infos[i].m_flag, infos[i].m_name,
                                 infos[i].m_port, idx);
        }
        if (infos[i].m_type == 5)
        {
            idx = infos[i].m_idx;
            if (idx == 0xff || idx != 0xcb)
                throw CDNFException("CServerHandler::Load() Guild Server Table Exception Break!");
            m_guildServer.Init(infos[i].m_flag, infos[i].m_name,
                               infos[i].m_port, idx);
        }
        if (infos[i].m_type == 7)
        {
            idx = infos[i].m_idx;
            if (idx != 0xcd)
                throw CDNFException("CServerHandler::Load() Statistics Server Table Exception Break!");
            m_statisticsServer.Init(infos[i].m_flag, infos[i].m_name,
                                    infos[i].m_port, idx);
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
        return m_gameServers + idx;
    CMyFileLog log(__FUNCTION__, 0xec);
    log("./log/GameServer.log", "Game Server Index Over Index : %d!\n", idx);
    return 0;
}
void CServerHandler::SendAllToGameServer(char* buf, int len)
{
    CGameServer* p = m_gameServers;
    for (int i = 0xff; i-- != 0; p++)
        p->SendToServer(buf, len);
}
CTcpServer* CServerHandler::CreateTcpServer(unsigned char idx, unsigned int port)
{
    CTcpServer* server = new CTcpServer;
    server->Init(port, m_app->Get_TcpNetSystem());
    server->SetServerType(idx);
    if (m_tcpServers.insert(std::make_pair(idx, server)).second)
        return server;
    delete server;
    return 0;
}
bool CServerHandler::DeleteTcpServer(unsigned char idx)
{
    std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.find(idx);
    if (it != m_tcpServers.end())
    {
        delete it->second;
        m_tcpServers.erase(it);
        CMyFileLog(__FUNCTION__, 0x130)("./log/TcpServer", "TcpServer(%d) Deleted", idx);
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
    CTcpServer* server = 0;
    char* buf = 0;
    for (std::map<unsigned char, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        server = it->second;
        if (server->IsValidServer())
        {
            buf = (char*)server->makePacketHeader(header->packetId, header->packetSize);
            memcpy((char*)buf + 0xa, (char*)header + 0xa, header->packetSize - 0xa);
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
