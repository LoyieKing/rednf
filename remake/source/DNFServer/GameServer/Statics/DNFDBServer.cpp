// df_statics_r — DNFDBServer（ORIG DNFDBServer.o 拆分）
#include <stdio.h>
#include <string.h>

#include "DNFDBServer.h"
#include "DNFUdpHandler.h"

STSpecCount::STSpecCount()
    : m_total(0)
{
}
Packet_DBMW_Save_Client_Spec_Statistic::Packet_DBMW_Save_Client_Spec_Statistic()
    : PacketHeader(0x9cf, 0x17e7), m_flag(0), m_count(0)
{
}
STErrorCount::STErrorCount()
{
    m_errorCode = 0;
    m_errorLine = 0;
    m_cnt = 0;
}
Packet_DBMW_Save_Error_Line_Statistic::Packet_DBMW_Save_Error_Line_Statistic()
    : PacketHeader(0x9d1, 0x17f6)
{
}
STSpecStatic::STSpecStatic(const HWSpec& spec)
{
    m_category1 = spec.m_category1;
    m_category2 = spec.m_category2;
    m_category3 = spec.m_category3;
}
STErrorStatic::STErrorStatic(const ErrorValue& value)
{
    m_errorLine = value.m_errorLine;
    m_errorCode = value.m_errorCode;
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
    if ((unsigned char)m_category1 < (unsigned char)other.m_category1) return true;
    if (m_category1 == other.m_category1)
    {
        if (m_category2 < other.m_category2) return true;
        if ((int)m_category2 == (int)other.m_category2 && m_category3 < other.m_category3) return true;
    }
    return false;
}
bool STErrorStatic::operator<(const STErrorStatic& other) const
{
    if (m_errorLine < other.m_errorLine) return true;
    if (m_errorLine == other.m_errorLine && m_errorCode < other.m_errorCode) return true;
    return false;
}
