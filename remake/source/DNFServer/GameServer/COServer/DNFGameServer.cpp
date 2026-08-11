// df_coserver_r — CGameServer（ORIG DNFGameServer.cpp）
#include <stdio.h>

#include "DNFGameServer.h"
#include "DNFUdpHandler.h"

CGameServer::CGameServer()
    : m_group(0xff), m_port(0), m_connectFlag(0), m_heartBeatCount(0x1e),
      m_heartBeatOver(0), m_channelNo(0), m_udp(0)
{
}

CGameServer::~CGameServer()
{
}

void CGameServer::Init(unsigned char group, std::string& name, unsigned short port,
                       unsigned char channel)
{
    m_group = group;
    m_name = name;
    m_port = port;
    m_udp = new CUdpHandler;
    m_udp->InitClientSocket();
    m_channelNo = channel;
}

bool CGameServer::IsValidServer()
{
    if (m_group == 0xff)
    {
        return false;
    }
    return true;
}

bool CGameServer::IsHeartBeatTimeOver()
{
    if (--m_heartBeatCount == 0)
    {
        if (++m_heartBeatOver > 0x14)
        {
            return true;
        }
        m_heartBeatCount = 0x1e;
    }
    return false;
}

void CGameServer::ResetHeartBeat()
{
    m_heartBeatCount = 0x1e;
    m_heartBeatOver = 0;
}

void CGameServer::SendToGameServer(char* buf, int len)
{
    if (m_group == 0xff)
    {
        return;
    }
    if (m_udp != 0)
    {
        m_udp->SendToServer(buf, len, m_port, m_name.c_str());
    }
}

void CGameServer::OnDisconnect()
{
    m_connectFlag = 0;
    m_heartBeatCount = 0x1e;
    m_heartBeatOver = 0;
}

bool CGameServer::IsConnected()
{
    return m_connectFlag;
}

void CGameServer::SetConnFlag(bool flag)
{
    m_connectFlag = flag;
}

unsigned char CGameServer::GetChannelNo()
{
    return m_channelNo;
}

unsigned char CGameServer::GetGroupNo()
{
    return m_group;
}
