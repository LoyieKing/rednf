#ifndef GAME_GOLDCARDEVENTSTATISTIC_H_
#define GAME_GOLDCARDEVENTSTATISTIC_H_

// ORIG ctor 0x85bee76（W，C2 同址）：+0x00 字节清 0、+0x01 int 清 0、+0x05 int 清 0。
// 字段非对齐 → packed；sizeof 0x9（ctor 仅清三字段，无其它布局证据）。
// 定义 OnlyOrigBatch4_Force.cpp。
#include <string.h>   // memset（ctor 三字段逐条清零，不用 memset）

#pragma pack(push, 1)
class GoldCardEventStatistic
{
public:
    // ORIG 0x85bee76 _ZN22GoldCardEventStatisticC1Ev (size 0x0000001f)
    GoldCardEventStatistic()
    {
        m_field0 = 0;
        m_field1 = 0;
        m_field5 = 0;
    }

    unsigned char m_field0;   // +0x00
    int m_field1;             // +0x01
    int m_field5;             // +0x05
};  // sizeof 0x9
#pragma pack(pop)

#endif  // GAME_GOLDCARDEVENTSTATISTIC_H_
