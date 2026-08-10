#ifndef DBMW_DNFPACKETTRACER_H_
#define DBMW_DNFPACKETTRACER_H_

#include "DBMWCommon.h"


class CPacketTracer
{
public:
    CPacketTracer();
    ~CPacketTracer();
    void AddLog(int type, int len);
    void WriteLog();
    void AbsoluteWriteLog();
    void ResetLog();
    void StartPacketProcessLog(unsigned int id);
    void EndPacketProcessLog(unsigned int id);
    void WritePacketProcessLog();
    void ResetPacketProcessLog();
    int m_field0;      // +0
    std::string m_log; // +4
    CUnixTimer* m_timer;  // +8
    std::map<unsigned int, stPacketProcess> m_processMap;  // +0xc
    int m_processCount;   // +0x24
};

#endif  // DBMW_DNFPACKETTRACER_H_
