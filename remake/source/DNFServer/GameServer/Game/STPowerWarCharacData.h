// STPowerWarCharacData：ORIG 仅导出弱内联 ctor W @0x086959dc；
// 依据反汇编：+0x0 word=0、+0x2 byte=0、+0x3 byte=1、+0x4 int=0、+0x8 int=0。
#ifndef DNF_GAME_STPOWERWARCHARACDATA_H_
#define DNF_GAME_STPOWERWARCHARACDATA_H_

struct STPowerWarCharacData
{
public:
    unsigned short m_data0;       // +0x0：ctor 置 0
    unsigned char m_data2;        // +0x2：ctor 置 0
    unsigned char m_data3;        // +0x3：ctor 置 1
    unsigned int m_data4;         // +0x4：ctor 置 0
    unsigned int m_data8;         // +0x8：ctor 置 0

    STPowerWarCharacData()
        : m_data0(0), m_data2(0), m_data3(1), m_data4(0), m_data8(0) {}   // ORIG W @0x086959dc（内联化）
};

#endif  // DNF_GAME_STPOWERWARCHARACDATA_H_
