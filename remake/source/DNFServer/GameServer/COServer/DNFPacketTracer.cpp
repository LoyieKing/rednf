// df_coserver_r — CPacketTracer（ORIG DNFPacketTracer.cpp）
#include <stdio.h>
#include <time.h>

#include "DNFPacketTracer.h"
#include "DNFFileLog.h"

CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}

CPacketTracer::CPacketTracer()
    : m_count(0)
{
}

CPacketTracer::~CPacketTracer()
{
}

void CPacketTracer::AddLog(int p1, int p2)
{
    time_t t;
    time(&t);
    tm t2 = *localtime(&t);
    char buf[32] = {0};
    sprintf(buf, "(%02d:%02d:%02d/%d/%d)", t2.tm_hour, t2.tm_min, t2.tm_sec, p2, p1);
    m_log += buf;
    m_count++;
}

void CPacketTracer::ResetLog()
{
    m_log.clear();
}

void CPacketTracer::WriteLog()
{
    if (m_count % 0x1e == 0)
    {
        register const char* s = m_log.c_str();
        DNF_LOG_SCOPE_LINE(0x2a, "./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", s);
        ResetLog();
    }
}

void CPacketTracer::AbsoluteWriteLog()
{
    register const char* s = m_log.c_str();
    DNF_LOG_SCOPE_LINE(0x32, "./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", s);
    ResetLog();
}
