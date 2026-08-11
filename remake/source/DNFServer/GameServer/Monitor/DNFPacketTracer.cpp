// df_monitor_r — DNFPacketTracer（从 MonitorTypes/App/Table 拆分）
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

#include "DNFPacketTracer.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CPacketTracer::CPacketTracer() : m_count(0), m_str() {}

CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}

void CPacketTracer::AddLog(int a, int b)
{
    time_t t;
    time(&t);
    struct tm st = *localtime(&t);
    char buf[32];
    sprintf(buf, "(%02d:%02d:%02d/%d/%d)", st.tm_hour, st.tm_min, st.tm_sec, b, a);
    m_str += buf;
    m_count = m_count + 1;
}

void CPacketTracer::WriteLog()
{
    if (m_count == (m_count / 0x1e) * 0x1e)
    {
        DNF_LOG_SCOPE_LINE(0x2e, "./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", m_str.c_str());
        ResetLog();
    }
}

void CPacketTracer::AbsoluteWriteLog()
{
    DNF_LOG_SCOPE_LINE(0x36, "./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", m_str.c_str());
    ResetLog();
}

void CPacketTracer::ResetLog()
{
    m_str.clear();
}
