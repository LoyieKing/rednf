#include "PacketTracer.h"

// ORIG 0x08260628（_ZN13CPacketTracerC1Ev，C2 同址）：
//   movl $0x0,(%eax) ; movl $0x0,0x4(%eax) —— 仅清零两计数器。
CPacketTracer::CPacketTracer()
{
    m_tcpCount = 0;
    m_udpCount = 0;
}

// ORIG 0x085992c8（_ZN13CPacketTracer9AddTcpLogEii）：
//   idx = m_tcpCount % 10（无符号魔数除法）；
//   m_logs[idx].m_flag = (char)flag; m_logs[idx].m_value = value;
//   ++m_tcpCount。
void CPacketTracer::AddTcpLog(int flag, int value)
{
    int idx = m_tcpCount % 10;

    m_logs[idx].m_flag = static_cast<char>(flag);
    m_logs[idx].m_value = value;

    ++m_tcpCount;
}

// ORIG 0x0859926c（_ZN13CPacketTracer9AddUdpLogEii）：
//   同 AddTcpLog，但计数器取 +0x04、记录区偏移 +10（logs[count%10+10]）。
void CPacketTracer::AddUdpLog(int flag, int value)
{
    int idx = m_udpCount % 10;

    m_logs[idx + 10].m_flag = static_cast<char>(flag);
    m_logs[idx + 10].m_value = value;

    ++m_udpCount;
}

// ORIG 0x08260640（_Z21CPacketTracerInstancev，W）：
//   __cxa_guard_acquire(0x940b598) → C1(&0x940b5a0) → __cxa_guard_release，
//   即函数局部静态单例；guard/实例符号名见头注释。
CPacketTracer* CPacketTracerInstance()
{
    static CPacketTracer instance;
    return &instance;
}
