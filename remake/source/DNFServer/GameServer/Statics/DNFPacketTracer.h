#ifndef DNF_PACKET_TRACER_H_
#define DNF_PACKET_TRACER_H_

#include <string>

// ---- CPacketTracer：count@0 + string@4 ----
class CPacketTracer
{
public:
    CPacketTracer();
    ~CPacketTracer();
    void AddLog(int p1, int p2);
    void ResetLog();
    void WriteLog();
    void AbsoluteWriteLog();
    int m_count;
    std::string m_log;
};

CPacketTracer* CPacketTracerInstance();

#endif // DNF_PACKET_TRACER_H_
