#ifndef DNF_PACKET_TRACER_H_
#define DNF_PACKET_TRACER_H_

#include <string>

// ---- CPacketTracer ----
class CPacketTracer
{
public:
    CPacketTracer();
    ~CPacketTracer();
    void AddLog(int type, int len);
    void WriteLog();
    void AbsoluteWriteLog();
    void ResetLog();
    unsigned int m_field0;  // +0（ORIG 无符号除法：mul/shr 形态）
    std::string m_log; // +4
};

CPacketTracer* CPacketTracerInstance();

#endif  // DNF_PACKET_TRACER_H_
