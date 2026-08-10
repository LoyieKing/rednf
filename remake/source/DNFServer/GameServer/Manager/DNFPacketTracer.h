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
    int m_field0;      // +0
    std::string m_log; // +4
};

CPacketTracer* CPacketTracerInstance();

#endif  // DNF_PACKET_TRACER_H_
