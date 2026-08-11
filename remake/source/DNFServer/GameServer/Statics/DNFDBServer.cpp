// df_statics_r — DNFDBServer（ORIG DNFDBServer.o 拆分）
#include <stdio.h>
#include <string.h>

#include "DNFDBServer.h"
#include "DNFUdpHandler.h"

STSpecCount::STSpecCount()
    : m_field0(0)
{
}
Packet_DBMW_Save_Client_Spec_Statistic::Packet_DBMW_Save_Client_Spec_Statistic()
    : PacketHeader(0x9cf, 0x17e7), m_fieldA(0), m_fieldB(0)
{
}
STErrorCount::STErrorCount()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field6 = 0;
}
Packet_DBMW_Save_Error_Line_Statistic::Packet_DBMW_Save_Error_Line_Statistic()
    : PacketHeader(0x9d1, 0x17f6)
{
}
STSpecStatic::STSpecStatic(const HWSpec& spec)
{
    m_field0 = spec.m_field0;
    m_field4 = spec.m_field4;
    m_field8 = spec.m_field8;
}
STErrorStatic::STErrorStatic(const ErrorValue& value)
{
    m_field0 = value.m_field0;
    m_field4 = value.m_field4;
}
CDBServer::CDBServer() : m_id(0xff)
{
    m_port = 0;
    m_connectFlag = 0;
    m_heartBeatCount = 0x14;
    m_heartBeatOver = 0;
    m_reserved = 0;
    m_udp = 0;
}
CDBServer::~CDBServer()
{
}
void CDBServer::Init(unsigned char id, std::string& name, unsigned short port,
                     unsigned char param)
{
    m_id = id;
    m_name = name;
    m_port = port;
    m_udp = new CUdpHandler;
    m_udp->InitClientSocket();
    m_reserved = param;
}
bool CDBServer::IsValidServer()
{
    if (m_id == 0xff)
    {
        return false;
    }
    return true;
}
bool CDBServer::IsHeartBeatTimeOver()
{
    if ((--m_heartBeatCount) == 0)
    {
        if ((++m_heartBeatOver) > 0x14)
        {
            return 1;
        }
        m_heartBeatCount = 0x14;
    }
    return 0;
}
void CDBServer::ResetHeartBeat()
{
    m_heartBeatCount = 0x14;
    m_heartBeatOver = 0;
}
void CDBServer::SendToServer(char* buf, int len)
{
    if (m_udp != 0)
    {
        m_udp->SendToServer(buf, len, m_port, m_name.c_str());
    }
}
void CDBServer::OnDisconnect()
{
    m_connectFlag = 0;
    m_heartBeatCount = 0x14;
    m_heartBeatOver = 0;
}
bool CDBServer::IsConnected()
{
    return m_connectFlag;
}
void CDBServer::SetConnFlag(bool flag)
{
    m_connectFlag = flag;
}
bool STSpecStatic::operator<(const STSpecStatic& other) const
{
    if ((unsigned char)m_field0 < (unsigned char)other.m_field0) return true;
    if (m_field0 == other.m_field0)
    {
        if (m_field4 < other.m_field4) return true;
        if ((int)m_field4 == (int)other.m_field4 && m_field8 < other.m_field8) return true;
    }
    return false;
}
bool STErrorStatic::operator<(const STErrorStatic& other) const
{
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0 && m_field4 < other.m_field4) return true;
    return false;
}
