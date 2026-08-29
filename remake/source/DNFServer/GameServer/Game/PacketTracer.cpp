#include "PacketTracer.h"
#include "GameTypes.h"
#include <cstdio>
#include <cstring>

CPacketTracer::CPacketTracer()
{
    m_tcpCount = 0;
    m_udpCount = 0;
}

void CPacketTracer::AddTcpLog(int flag, int value)
{
    int idx = m_tcpCount % 10;

    m_logs[idx].m_flag = static_cast<char>(flag);
    m_logs[idx].m_value = value;
    ++m_tcpCount;
}

void CPacketTracer::AddUdpLog(int flag, int value)
{
    int idx = m_udpCount % 10;

    m_logs[idx + 10].m_flag = static_cast<char>(flag);
    m_logs[idx + 10].m_value = value;
    ++m_udpCount;
}

CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}

void CPacketTracer::AbsoluteTcpWriteLog()
{
    char buf[0x800] = { '\0' };
    for (int i = 0; i <= 9; ++i)
    {
        std::sprintf(buf, "%s,%d/%d", buf,
                     static_cast<int>(static_cast<unsigned char>(m_logs[i].m_flag)),
                     m_logs[i].m_value);
    }
    --m_tcpCount;
    register unsigned int idx = m_tcpCount % 10;
    cMyTrace trace(__FUNCTION__, 0x41, 0);
    trace("[TRACE_TCP_PACKET] (idx:%d)%s", idx, buf);
}

void CPacketTracer::AbsoluteUdpWriteLog()
{
    char buf[0x800] = { '\0' };
    for (int i = 0; i <= 9; ++i)
    {
        std::sprintf(buf, "%s,%d/%d", buf,
                     static_cast<int>(static_cast<unsigned char>(m_logs[i + 10].m_flag)),
                     m_logs[i + 10].m_value);
    }
    --m_udpCount;
    register unsigned int idx = m_udpCount % 10;
    cMyTrace trace(__FUNCTION__, 0x5a, 0);
    trace("[TRACE_UDP_PACKET] (idx:%d)%s", idx, buf);
}
