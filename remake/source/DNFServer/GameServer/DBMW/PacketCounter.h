#ifndef DBMW_PACKETCOUNTER_H_
#define DBMW_PACKETCOUNTER_H_

#include "DBMWCommon.h"


template<int Lo, int Hi>
class CPacketCounter
{
public:
    CPacketCounter(char* dir, char* name);
    ~CPacketCounter();
    void Reset();
    void IncrementPacketCount(int id);
    void BeforeProcess();
    void AfterProcess(int id);
    char m_data[0x1d648];  // +0
};

#endif  // DBMW_PACKETCOUNTER_H_
