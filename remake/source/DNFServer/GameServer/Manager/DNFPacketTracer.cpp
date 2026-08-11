// df_manager_r — CPacketTracer（ORIG DNFPacketTracer.cpp）
#include "DNFPacketTracer.h"

#include <stdio.h>
#include <string.h>

#include "DNFFileLog.h"

CPacketTracer::CPacketTracer() : m_field0(0), m_log() {}

CPacketTracer::~CPacketTracer() {}

void CPacketTracer::AbsoluteWriteLog()
{
    DNF_LOG_SCOPE_LINE(0x2e, "./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", m_log.c_str());
    ResetLog();
}

void CPacketTracer::WriteLog()
{
    if (m_field0 % 30 == 0)
    {
        DNF_LOG_SCOPE_LINE(0x26, "./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", m_log.c_str());
        ResetLog();
    }
}

void CPacketTracer::AddLog(int type, int len)
{
    char buf[0x20] = {0};
    sprintf(buf, "(%d/%d)", len, type);
    m_log += buf;
    m_field0++;
}

CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}

void CPacketTracer::ResetLog() { m_log.clear(); }
