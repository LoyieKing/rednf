// df_dbmw_r - DNFPacketTracer (ORIG DNFPacketTracer.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

CPacketTracer::CPacketTracer()
{
    m_field0 = 0;
    m_timer = 0;
    m_processCount = 0;
    m_timer = new CUnixTimer;
    ResetPacketProcessLog();
}
CPacketTracer::~CPacketTracer()
{
    delete m_timer;
}
void CPacketTracer::WriteLog()
{
    if (m_field0 % 30 == 0)
    {
        CMyFileLog log("WriteLog", 0x37);
        log("./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", m_log.c_str());
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
void CPacketTracer::AbsoluteWriteLog()
{
    CMyFileLog log("AbsoluteWriteLog", 0x3f);
    log("./log/packet_trace", "[TRACE_PACKET] Packet Code : %s\n", m_log.c_str());
    ResetLog();
}
CPacketTracer* CPacketTracerInstance() { static CPacketTracer instance; return &instance; }
void CPacketTracer::ResetLog() { m_log.clear(); }
void CPacketTracer::StartPacketProcessLog(unsigned int id)
{
    m_timer->SetLastTime();
    std::map<unsigned int, stPacketProcess>::iterator it =
        m_processMap.find(id);
    if (it == m_processMap.end())
    {
        stPacketProcess p;
        p.m_accTime = 0.0;
        p.m_count = 0;
        m_processMap.insert(std::make_pair(id, p));
    }
}
void CPacketTracer::EndPacketProcessLog(unsigned int id)
{
    std::map<unsigned int, stPacketProcess>::iterator it =
        m_processMap.find(id);
    if (it != m_processMap.end())
    {
        it->second.m_count += 1;
        it->second.m_accTime += 1000.0 * m_timer->GetTimeInterval();
    }
}
void CPacketTracer::WritePacketProcessLog()
{
    m_processCount -= 1;
    if (m_processCount < 1)
    {
        for (std::map<unsigned int, stPacketProcess>::iterator it =
                 m_processMap.begin();
             it != m_processMap.end(); ++it)
        {
            if (it->second.m_count != 0)
            {
                CMyFileLog log("WritePacketProcessLog", 0x6f);
                log("./log/PacketProcess",
                    "id(%d), acc count(%d), acc time(%.4f ms), average time(%4.4f ms)",
                    it->first, it->second.m_count, it->second.m_accTime,
                    it->second.m_accTime / (double)it->second.m_count);
            }
        }
        ResetPacketProcessLog();
    }
}
void CPacketTracer::ResetPacketProcessLog()
{
    m_processMap.clear();
    m_processCount = 0x1e;
}
