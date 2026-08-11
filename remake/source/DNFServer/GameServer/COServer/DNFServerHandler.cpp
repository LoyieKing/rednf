// df_coserver_r — CServerHandler（ORIG DNFServerHandler.cpp）
#include <stdio.h>

#include "DNFApplication.h"
#include "DNFServerHandler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "PacketHeader.h"

CServerHandler::CServerHandler()
{
    // m_servers 就地构造（与原始显式循环等价）
    m_app = 0;
}

CServerHandler::~CServerHandler()
{
}

void CServerHandler::Attach(CApplication* app)
{
    if (app != 0)
    {
        m_app = app;
    }
}

void CServerHandler::Load(ST_ServerInfo* info)
{
    // ORIG 实测槽位：index@-0x1e / group@-0x1d / i@-0x1c（声明序 先 index、
    // group、后 i）。循环变量声明在 index/group 之后才能复现该布局
    // （GCC 4.4 O0 按声明序分配：先声明者取低地址）。
    unsigned char index;
    unsigned char group;
    int i;
    for (i = 0; i < 0x649b; i++)
    {
        if (info[i].m_field0 == 1)
        {
            index = info[i].m_field2;
            group = info[i].m_field1;
            if (index == 0xff)
            {
                throw CDNFException("CServerHandler::Load() Server Table Exception Break! "
                                    "bServerIndex >= MAX_CONN_SERVER");
            }
            if (100 < group)
            {
                throw CDNFException("CServerHandler::Load() Server Table Exception Break! "
                                    "bServerGroup >= SERVER_GROUP_MAX");
            }
            m_servers[(int)group * 0xff + (int)index].Init(group, info[i].m_string,
                                                           info[i].m_ushort, index);
        }
    }
}

void CServerHandler::Process()
{
    CGameServer* p = m_servers;
    int left = 0x649b;
    int counter = 0;
    while (left-- != 0)
    {
        counter++;
        if (!p->IsValidServer())
        {
            p++;
            continue;
        }
        if (p->IsConnected())
        {
            if (p->IsHeartBeatTimeOver())
            {
                p->OnDisconnect();
                // ORIG 实测（0x8056702）：无 groupNo/channelNo 局部槽，
                // GetGroupNo/GetChannelNo 结果直接 movzbl 到 esi/ebx 供日志实参。
                DNF_LOG_SCOPE_LINE(0x5e, "./log/GameServer",
                    "Game Server Disconnect, Index : %d, channel no : %d, group no: %d\n",
                    counter, p->GetChannelNo() & 0xff, p->GetGroupNo() & 0xff);
            }
        }
        p++;
    }
}

void CServerHandler::ResetHeartBeat(unsigned char group, unsigned char index)
{
    if (index != 0xff && group < 0x65 &&
        m_servers[(int)group * 0xff + (int)index].IsValidServer())
    {
        m_servers[(int)group * 0xff + (int)index].ResetHeartBeat();
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x7a,"./log/GameServer",
            "CServerHandler::ResetHeartBeat\tGame Server Index Over(%d), Group Index Over(%d)",
            (int)index, (int)group);
    }
}

void CServerHandler::SetGameServerIpPort(unsigned char id, unsigned int ip,
                                         unsigned short port)
{
    // 原始即空实现
}

bool CServerHandler::IsConnectedGameServer(unsigned char group, unsigned char index)
{
    if (index != 0xff && group < 0x65 &&
        m_servers[(int)group * 0xff + (int)index].IsValidServer())
    {
        return m_servers[(int)group * 0xff + (int)index].IsConnected();
    }
    DNF_LOG_SCOPE_LINE(0xa4,"./log/GameServer",
        "CServerHandler::IsConnectedGameServer\tGame Server Index Over(%d), Group Index Over(%d)",
        (int)index, (int)group);
    return 0;
}

void CServerHandler::SetConnectFlag(unsigned char group, unsigned char index, bool flag)
{
    if (index != 0xff && group < 0x65 &&
        m_servers[(int)group * 0xff + (int)index].IsValidServer())
    {
        // 原始：SetConnFlag 目标地址按 this + group*0x13ec + index*0x14 形态计算
        ((CGameServer*)((char*)this + (int)group * 0x13ec + (int)index * 0x14))
            ->SetConnFlag(flag);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xbc,"./log/GameServer",
            "CServerHandler::SetConnectFlag\tGame Server Index Over(%d), Group Index Over(%d)",
            (int)index, (int)group);
    }
}

CGameServer* CServerHandler::GetGameServer(int idx)
{
    if (idx < 0x649b && m_servers[idx].IsValidServer())
    {
        return m_servers + idx;
    }
    DNF_LOG_SCOPE_LINE(0xdc,"./log/GameServer", "CServerHandler::GetGameServer\tGame Server Index Over Index : %d!\n",
        idx);
    return 0;
}

void CServerHandler::SendToGameServer(unsigned char group, unsigned char index,
                                      PacketHeader* header)
{
    CGameServer* gs = GetGameServer((int)group * 0xff + (int)index);
    if (gs != 0)
    {
        gs->SendToGameServer((char*)header, header->packetSize);
    }
}

void CServerHandler::SendAllToGameServer(char* buf, int len)
{
    CGameServer* p = m_servers;
    int left = 0x649b;
    while (left-- != 0)
    {
        if (p->IsValidServer())
        {
            p->SendToGameServer(buf, len);
        }
        p++;
    }
}
