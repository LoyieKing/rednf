// df_monitor_r — ServerLoadChecker（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
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

#include "ServerLoadChecker.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"

char CAppLoadChecker::CheckTcpRecvQ(int size)
{
    if (checkTcpRecvLoad(size))
    {
        setTcpRecvQueue(size);
        return 1;
    }
    return 0;
}

char CAppLoadChecker::CheckUdpRecvQ(int size)
{
    if (checkUdpRecvLoad(size))
    {
        setUdpRecvQueue(size);
        return 1;
    }
    return 0;
}

char CAppLoadChecker::CheckTcpSendQ(int size)
{
    if (checkTcpSendLoad(size))
    {
        setTcpSendQueue(size);
        return 1;
    }
    return 0;
}

void CAppLoadChecker::setTcpRecvQueue(int size) { m_tcpRecvLast = size; }

void CAppLoadChecker::setUdpRecvQueue(int size) { m_udpRecvLast = size; }

void CAppLoadChecker::setTcpSendQueue(int size) { m_tcpSendLast = size; }

void CAppLoadChecker::RequestDB(CServerHandler* serverHandler, int flag, int size)
{
    Packet_Server_Queue_Load_Statistic pkt;
    pkt.m_fieldA = 0xc9;
    pkt.m_fieldB = (unsigned char)flag;
    pkt.m_fieldC = (unsigned short)size;
    serverHandler->SendToDB(&pkt);
}

CAppLoadChecker::CAppLoadChecker()
{
    m_tcpRecvLast = 0;
    m_udpRecvLast = 0;
    m_tcpSendLast = 0;
    m_tcpRecvLevel = 0;
    m_udpRecvLevel = 0;
    m_tcpSendLevel = 0;
}

CAppLoadChecker* CAppLoadCheckerInstance()
{
    static CAppLoadChecker inst;
    return &inst;
}

int CAppLoadChecker::checkTcpRecvLoad(int size)
{
    char level = m_tcpRecvLevel;
    if (level < 1 && 0x32 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 1;
        return 1;
    }
    if (level < 2 && 100 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 2;
        return 1;
    }
    if (level < 3 && 200 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 3;
        return 1;
    }
    if (level < 4 && 500 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 4;
        return 1;
    }
    if (level < 5 && 1000 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 5;
        return 1;
    }
    if (level < 6 && 5000 < size - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 6;
        return 1;
    }
    if (level == 6 && 5000 < size - m_tcpRecvLast)
    {
        return 1;
    }
    if (level < 0 || m_tcpRecvLast - size < 0x33)
    {
        if (level < -1 || m_tcpRecvLast - size < 0x65)
        {
            if (level < -2 || m_tcpRecvLast - size < 0xc9)
            {
                if (level < -3 || m_tcpRecvLast - size < 0x1f5)
                {
                    if (level < -4 || m_tcpRecvLast - size < 0x3e9)
                    {
                        if (level < -5 || m_tcpRecvLast - size < 0x1389)
                        {
                            if (level == 0xfa && 5000 < m_tcpRecvLast - size)
                            {
                                return 1;
                            }
                            return 0;
                        }
                        m_tcpRecvLevel = 0xfa;
                        return 1;
                    }
                    m_tcpRecvLevel = 0xfb;
                    return 1;
                }
                m_tcpRecvLevel = 0xfc;
                return 1;
            }
            m_tcpRecvLevel = 0xfd;
            return 1;
        }
        m_tcpRecvLevel = 0xfe;
        return 1;
    }
    m_tcpRecvLevel = 0xff;
    return 1;
}

int CAppLoadChecker::checkUdpRecvLoad(int size)
{
    char level = m_udpRecvLevel;
    if (level < 1 && 0x32 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 1;
        return 1;
    }
    if (level < 2 && 100 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 2;
        return 1;
    }
    if (level < 3 && 200 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 3;
        return 1;
    }
    if (level < 4 && 500 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 4;
        return 1;
    }
    if (level < 5 && 1000 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 5;
        return 1;
    }
    if (level < 6 && 5000 < size - m_udpRecvLast)
    {
        m_udpRecvLevel = 6;
        return 1;
    }
    if (level == 6 && 5000 < size - m_udpRecvLast)
    {
        return 1;
    }
    if (level < 0 || m_udpRecvLast - size < 0x33)
    {
        if (level < -1 || m_udpRecvLast - size < 0x65)
        {
            if (level < -2 || m_udpRecvLast - size < 0xc9)
            {
                if (level < -3 || m_udpRecvLast - size < 0x1f5)
                {
                    if (level < -4 || m_udpRecvLast - size < 0x3e9)
                    {
                        if (level < -5 || m_udpRecvLast - size < 0x1389)
                        {
                            if (level == 0xfa && 5000 < m_udpRecvLast - size)
                            {
                                return 1;
                            }
                            return 0;
                        }
                        m_udpRecvLevel = 0xfa;
                        return 1;
                    }
                    m_udpRecvLevel = 0xfb;
                    return 1;
                }
                m_udpRecvLevel = 0xfc;
                return 1;
            }
            m_udpRecvLevel = 0xfd;
            return 1;
        }
        m_udpRecvLevel = 0xfe;
        return 1;
    }
    m_udpRecvLevel = 0xff;
    return 1;
}

int CAppLoadChecker::checkTcpSendLoad(int size)
{
    char level = m_tcpSendLevel;
    if (level < 1 && 0x32 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 1;
        return 1;
    }
    if (level < 2 && 100 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 2;
        return 1;
    }
    if (level < 3 && 200 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 3;
        return 1;
    }
    if (level < 4 && 500 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 4;
        return 1;
    }
    if (level < 5 && 1000 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 5;
        return 1;
    }
    if (level < 6 && 5000 < size - m_tcpSendLast)
    {
        m_tcpSendLevel = 6;
        return 1;
    }
    if (level == 6 && 5000 < size - m_tcpSendLast)
    {
        return 1;
    }
    if (level < 0 || m_tcpSendLast - size < 0x33)
    {
        if (level < -1 || m_tcpSendLast - size < 0x65)
        {
            if (level < -2 || m_tcpSendLast - size < 0xc9)
            {
                if (level < -3 || m_tcpSendLast - size < 0x1f5)
                {
                    if (level < -4 || m_tcpSendLast - size < 0x3e9)
                    {
                        if (level < -5 || m_tcpSendLast - size < 0x1389)
                        {
                            if (level == 0xfa && 5000 < m_tcpSendLast - size)
                            {
                                return 1;
                            }
                            return 0;
                        }
                        m_tcpSendLevel = 0xfa;
                        return 1;
                    }
                    m_tcpSendLevel = 0xfb;
                    return 1;
                }
                m_tcpSendLevel = 0xfc;
                return 1;
            }
            m_tcpSendLevel = 0xfd;
            return 1;
        }
        m_tcpSendLevel = 0xfe;
        return 1;
    }
    m_tcpSendLevel = 0xff;
    return 1;
}

