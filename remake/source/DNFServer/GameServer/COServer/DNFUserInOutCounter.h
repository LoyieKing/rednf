#ifndef DNF_USER_IN_OUT_COUNTER_H_
#define DNF_USER_IN_OUT_COUNTER_H_

#include "DNFTableBase.h"

// ---- CDNFUserInOutCounter：0x324d9 ----
// counters[101][255][2]（in@0/out@4，共 0x324d8）+ tick@0x324d8

class CDNFUserInOutCounter
{
public:
    CDNFUserInOutCounter();
    ~CDNFUserInOutCounter();
    void Reset();
    void WriteLog();
    void ProcessWrite();
    void IncUserIn(int group, int index);
    void IncUserOut(int group, int index);
    int m_counters[101][255][2];
    unsigned char m_tick;   // +0x324d8
};

#endif // DNF_USER_IN_OUT_COUNTER_H_
