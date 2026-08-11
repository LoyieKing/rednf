// df_statics_r — DNFManagerServer（ORIG DNFManagerServer.o 拆分）
#include <stdio.h>

#include "DNFManagerServer.h"
#include "DNFUdpHandler.h"
#include "Packet_Monitor_UDP_HeartBeat.h"

CManagerServer::CManagerServer() : m_id(0xff)
{
    m_port = 0;
    m_connectFlag = 0;
    m_param = 0;
    m_udp = 0;
}
CManagerServer::~CManagerServer()
{
}
void CManagerServer::Init(unsigned char id, std::string& name, unsigned short port,
                          unsigned char param)
{
    m_id = id;
    m_name = name;
    m_port = port;
    m_udp = new CUdpHandler;
    m_udp->InitClientSocket();
    m_param = param;
}
bool CManagerServer::IsValidServer()
{
    if (m_id == 0xff)
    {
        return false;
    }
    return true;
}
int CManagerServer::SendToServer(char* buf, int len)
{
    if (m_udp != 0)
    {
        return m_udp->SendToServer(buf, len, m_port, m_name.c_str());
    }
    return 0;
}
void CManagerServer::OnDisconnect()
{
    m_connectFlag = 0;
}
void CManagerServer::SendHeartBeat(int group)
{
    if (m_udp != 0)
    {
        Packet_Monitor_UDP_HeartBeat pkt;
        pkt.m_fieldA = (char)group;
        m_udp->SendToServer((char*)&pkt, 0xb, m_port, m_name.c_str());
    }
}
void CManagerServer::SetConnFlag(bool flag)
{
    m_connectFlag = flag;
}
