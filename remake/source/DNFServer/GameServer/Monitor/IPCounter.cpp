// df_monitor_r — IPCounter（从 MonitorTypes/App/Table 拆分）
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

#include "IPCounter.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFServerHandler.h"

CIPCounter::CIPCounter() {}

CIPCounter::~CIPCounter() {}

void CIPCounter::Init(CServerHandler* handler)
{
    m_count = 0;
    m_min = 0;
    m_term = 0x708;
    m_option = 5;
    m_field10 = 1;
    m_field11 = 1;
    m_handler = handler;
}

void CIPCounter::Proc(unsigned int tick)
{
    if (m_min >= tick)
    {
        return;
    }
    if (m_field10 == 0)
    {
        return;
    }
    if (m_count + m_term >= tick)
    {
        return;
    }
    m_handler->SendDBMWRequestIPCounter(m_field11, m_option);
    m_count = tick;
    CMyFileLog log("Proc", 0x3c);
    log("./log/Secu", "[IP Counter] LoadStart");
    m_min = tick + 0x3c;
}

void CIPCounter::setLoadTerm(unsigned char term)
{
    unsigned int v = (unsigned int)term * 0x3c;
    if (0x2a30 < v)
    {
        v = 0x2a30;
    }
    *(unsigned int*)((char*)this + 0xc) = v;
    v = *(unsigned int*)((char*)this + 0xc);
    if (v < 0x708)
    {
        v = 0x708;
    }
    *(unsigned int*)((char*)this + 0xc) = v;
}

void CIPCounter::setMinIPCount(unsigned char count)
{
    if (200 < count)
    {
        count = 200;
    }
    *(unsigned char*)this = count;
}

void CIPCounter::setOption(unsigned char type, unsigned char opt)
{
    if (type == 0)
    {
        setLoadTerm(opt);
    }
    else if (type == 1)
    {
        setMinIPCount(opt);
    }
    else if (type == 2)
    {
        *(unsigned char*)((char*)this + 0x10) = 1;
        *(unsigned int*)((char*)this + 8) = 0;
        *(unsigned int*)((char*)this + 4) = 0;
    }
    else if (type == 3)
    {
        *(unsigned char*)((char*)this + 0x10) = 0;
    }
    else if (type == 4)
    {
        *(unsigned char*)((char*)this + 0x11) = 0;
    }
}

