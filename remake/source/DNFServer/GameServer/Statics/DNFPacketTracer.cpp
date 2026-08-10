// df_statics_r — DNFPacketTracer（ORIG DNFPacketTracer.o 拆分）
#include <stdio.h>
#include <time.h>

#include "DNFPacketTracer.h"
#include "DNFFileLog.h"

CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}
void CPacketTracer::AddLog(int p1, int p2)
{
    time_t t = time(0);
    tm* pt = localtime(&t);
    char buf[32];
    char* p = buf;
    for (unsigned int i = 0; i < 0x20; i += 4)
    {
        p[i] = '\0';
        p[i + 1] = '\0';
        p[i + 2] = '\0';
        p[i + 3] = '\0';
    }
    sprintf(buf, "(%02d:%02d:%02d/%d/%d)", pt->tm_hour, pt->tm_min, pt->tm_sec, p2, p1);
    m_log += buf;
    m_count++;
}
void CPacketTracer::ResetLog()
{
    m_log.clear();
}
void CPacketTracer::WriteLog()
{
    if (m_count == (int)((unsigned int)m_count / 0x1e) * 0x1e)
    {
        register const char* trace = m_log.c_str();
        CMyFileLog log("WriteLog", 0x2a);
        log("./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", trace);
        ResetLog();
    }
}
void CPacketTracer::AbsoluteWriteLog()
{
    register const char* trace = m_log.c_str();
    CMyFileLog log("AbsoluteWriteLog", 0x32);
    log("./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", trace);
    ResetLog();
}
CPacketTracer::CPacketTracer()
{
    m_count = 0;
}
CPacketTracer::~CPacketTracer()
{
}
