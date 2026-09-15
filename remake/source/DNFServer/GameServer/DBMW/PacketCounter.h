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
    union
    {
        struct
        {
            unsigned int m0;         // +0（P342：ORIG 成员符号性为 unsigned）
            time_t m_time;           // +4
            unsigned int m_a1[0x2418]; // +8
            unsigned int m_a3[0x2418]; // +0x9068
            unsigned char m_b[0x2418]; // +0x120c8
            unsigned int m_a2[0x2418]; // +0x144e0
            char m_name[0x100];      // +0x1d540
            bool m_flag;             // +0x1d640
            char m_tail;             // +0x1d641
        };
        char m_data[0x1d664];        // +0（ORIG nm -S 该静态对象 = 0x1d664；尾 0x1c 无引用）
    };
};

#endif  // DBMW_PACKETCOUNTER_H_
