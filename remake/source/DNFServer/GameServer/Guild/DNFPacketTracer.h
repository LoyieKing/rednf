#ifndef DNFPACKETTRACER_H_
#define DNFPACKETTRACER_H_

#include <queue>
#include <string>
#include "PacketHeader.h"
#include "Thread.h"

#include "PacketHeader.h"

class CPacketTracer;

// from GuildPacket.h
class CPacketTracer
{
public:
    CPacketTracer();
    ~CPacketTracer();
    void AddLog(int p1, int p2);
    void ResetLog();
    void WriteLog();
    void AbsoluteWriteLog();
    int m_nCount;        // offset 0
    std::string m_strLog; // offset 4
};

CPacketTracer* CPacketTracerInstance();

#endif
