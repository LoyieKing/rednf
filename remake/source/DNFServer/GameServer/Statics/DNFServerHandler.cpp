// df_statics_r — DNFServerHandler（ORIG DNFServerHandler.o 拆分）
#include <stdio.h>

#include "DNFServerHandler.h"
#include "DNFApplication.h"
#include "DNFServerConfig.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFPacketTranslater.h"
#include "PacketHeader.h"

CServerHandler::CServerHandler() : m_app(0)
{
    m_reserved2 = 0;
}
CServerHandler::~CServerHandler()
{
}
void CServerHandler::Load(ST_ServerInfo* info)
{
    for (int i = 0; i < 0xff; i++)
    {
        if (info[i].m_field0 == 1)
        {
            unsigned char index = info[i].m_field2;
            if (index == 0xff)
            {
                throw CDNFException("CServerHandler::Load() Server Table Exception Break!");
            }
            m_servers[index].Init(info[i].m_field1, info[i].m_string, info[i].m_ushort, index);
        }
        if (info[i].m_field0 == 2)
        {
            unsigned char index = info[i].m_field2;
            if (index == 0xff || index != 0xc8)
            {
                printf("*******%d", index);
                throw CDNFException("CServerHandler::Load() DB2 Server Table Exception Break!");
            }
            m_dbServer.Init(info[i].m_field1, info[i].m_string, info[i].m_ushort, index);
        }
        if (info[i].m_field0 == 4)
        {
            unsigned char index = info[i].m_field2;
            if (index == 0xff || index != 0xca)
            {
                throw CDNFException("CServerHandler::Load() Manager Server Table Exception Break!");
            }
            m_mgrServer.Init(info[i].m_field1, info[i].m_string, info[i].m_ushort, index);
        }
    }
}
void CServerHandler::Process()
{
    int tick = m_reserved2;
    m_reserved2 = tick + 1;
    if (3 < tick)
    {
        m_mgrServer.SendHeartBeat(GetServerGroupNo() & 0xff);
        m_reserved2 = 0;
    }
    CGameServer* p = m_servers;
    int left = 0xff;
    while (left != 0)
    {
        left--;
        if (p->IsValidServer())
        {
            if (p->IsConnected())
            {
                if (p->IsHeartBeatTimeOver())
                {
                    p->OnDisconnect();
                    DNF_LOG_SCOPE_LINE(0x89, "./log/GameServer",
                        "CServerHandler::Process() Index : %d!\tCall User Info!\n", 0x100 - left);
                }
            }
        }
        p++;
    }
    if (m_dbServer.IsValidServer())
    {
        if (m_dbServer.IsConnected())
        {
            if (m_dbServer.IsHeartBeatTimeOver())
            {
                m_dbServer.OnDisconnect();
                DNF_LOG_SCOPE_LINE(0x9e, "./log/DBServerErr", "CServerHandler::Process() DB Server Down!\n");
            }
        }
    }
}
void CServerHandler::ResetHeartBeat(unsigned char index)
{
    if (index != 0xff && m_servers[index].IsValidServer())
    {
        m_servers[index].ResetHeartBeat();
    }
    else
    {
        register int idx = index;
        DNF_LOG_SCOPE_LINE(0xb4,"./log/GameServer", "CServerHandler::ResetHeartBeat\tGame Server Index Over Index : %d!\n",
            idx);
    }
}
void CServerHandler::ResetDBHeartBeat()
{
    m_dbServer.ResetHeartBeat();
}
void CServerHandler::SetGameServerIpPort(unsigned char id, unsigned int ip,
                                         unsigned short port)
{
}
char CServerHandler::IsConnectedGameServer(unsigned char index)
{
    if (index != 0xff && m_servers[index].IsValidServer())
    {
        return m_servers[index].IsConnected();
    }
    register int idx = index;
    DNF_LOG_SCOPE_LINE(0xe9,"./log/GameServer", "CServerHandler::IsConnectedGameServer\tGame Server Index Over Index : %d!\n",
        idx);
    return 0;
}
char CServerHandler::IsConnectedDBServer()
{
    return m_dbServer.IsConnected();
}
void CServerHandler::SetDBConnectFlag(bool flag)
{
    m_dbServer.SetConnFlag(flag);
}
void CServerHandler::SetManagerConnectFlag(bool flag)
{
    m_mgrServer.SetConnFlag(flag);
}
void CServerHandler::SetConnectFlag(unsigned char index, bool flag)
{
    if (index != 0xff && m_servers[index].IsValidServer())
    {
        m_servers[index].SetConnFlag(flag);
    }
    else
    {
        register int idx = index;
        DNF_LOG_SCOPE_LINE(0x125,"./log/GameServer", "CServerHandler::SetConnectFlag\tGame Server Index Over Index : %d!\n",
            idx);
    }
}
void CServerHandler::Attach(CApplication* app)
{
    if (app != 0)
    {
        m_app = app;
    }
}
CGameServer* CServerHandler::GetGameServer(int idx)
{
    if (idx < 0xff && m_servers[idx].IsValidServer())
    {
        return &m_servers[idx];
    }
    DNF_LOG_SCOPE_LINE(0x145,"./log/GameServer", "CServerHandler::GetGameServer\tGame Server Index Over Index : %d!\n",
        idx);
    return 0;
}
void CServerHandler::SendToGameServer(unsigned char index, PacketHeader* header)
{
    CGameServer* gs = GetGameServer(index);
    if (gs != 0)
    {
        gs->SendToGameServer((char*)header, header->packetSize);
    }
}
void CServerHandler::SendAllToGameServer(char* buf, int len)
{
    CGameServer* p = m_servers;
    int left = 0xff;
    while (left-- != 0)
    {
        if (p->IsValidServer())
        {
            p->SendToGameServer(buf, len);
        }
        p++;
    }
}
void CServerHandler::SendToDB(PacketHeader* header)
{
    m_dbServer.SendToServer((char*)header, header->packetSize);
}
unsigned char CServerHandler::GetServerGroupNo()
{
    return m_app->Get_ServerGroup();
}
void CServerHandler::SendToManager(PacketHeader* header)
{
    m_mgrServer.SendToServer((char*)header, header->packetSize);
}
namespace global_function
{
void SendPacketToDbmw(char* data)
{
    if (CPacketTranslater::m_pclApp != 0)
    {
        CPacketTranslater::m_pclApp->Get_ServerHandler()->SendToDB((PacketHeader*)data);
    }
}
}
