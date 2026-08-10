// df_statics_r — DNFGameServer（ORIG DNFGameServer.o 拆分）
#include <stdio.h>

#include "DNFGameServer.h"
#include "DNFUdpHandler.h"

CGameServer::CGameServer() : m_group(0xff)
{
    m_port = 0;
    m_connectFlag = 0;
    m_heartBeatCount = 0x14;
    m_heartBeatOver = 0;
    m_channelNo = 0;
    m_udp = 0;
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
int CGameServer::IsHeartBeatTimeOver()
{
    m_heartBeatCount = (char)(m_heartBeatCount - 1);
    if (m_heartBeatCount == 0)
    {
        m_heartBeatOver = (char)(m_heartBeatOver + 1);
        if (0x14 < m_heartBeatOver)
        {
            return 1;
        }
        m_heartBeatCount = 0x14;
    }
    return 0;
}
void CGameServer::ResetHeartBeat()
{
    m_heartBeatCount = 0x14;
    m_heartBeatOver = 0;
}
void CGameServer::SendToGameServer(char* buf, int len)
{
    if (m_group != 0xff && m_udp != 0)
    {
        m_udp->SendToServer(buf, len, m_port, m_name.c_str());
    }
}
void CGameServer::OnDisconnect()
{
    m_connectFlag = 0;
    m_heartBeatCount = 0x14;
    m_heartBeatOver = 0;
}
char CGameServer::IsConnected()
{
    return m_connectFlag;
}
void CGameServer::SetConnFlag(bool flag)
{
    m_connectFlag = flag;
}
CGameServer::~CGameServer()
{
}
