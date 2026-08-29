// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x08110bdc (C1 W) + 提取自 W8_EventClassifyB.cpp:94 定义段（原 cpp 内定义，未改 cpp）。
#ifndef DNF_GAME_PACKET_EMBLEM_CREATE_STATISTIC_H_
#define DNF_GAME_PACKET_EMBLEM_CREATE_STATISTIC_H_

#include <cstring>

#include "PacketHeader.h"

// ctor W 0x08110bdc：header(0x17a4,0x36) 后清零 dword@+0xa。ORIG 循环内写入
// 地址为 -0x44+4i(value/条数位) 与 -0x40+4i(grade)：相邻槽交错共享
// （value_{i+1} 与 grade_i 同槽）。以槽数组原样建模：slots[0] 尾写条数、
// slots[j]=value_j、slots[j+1]=grade_j。
class Packet_Emblem_Create_Statistic : public PacketHeader
{
public:
    Packet_Emblem_Create_Statistic()
        : PacketHeader(static_cast<unsigned short>(0x17a4),
                       static_cast<unsigned short>(0x36))
    {
        m_slots[0] = 0;          // ORIG 只清零 count 槽 dword@+0xa（0x8110bdc 实证）
    }

    void put_value(int i, int v) { m_slots[i] = v; }         // slot j=i
    void put_grade(int i, int g) { m_slots[i + 1] = g; }     // slot j=i+1
    void put_count(int n) { m_slots[0] = n; }                // 尾写覆盖首槽

    int m_slots[11];             // +0x0a..（0x2c 字节 = 11×int）
} __attribute__((packed));       // sizeof == 0x36

#endif  // DNF_GAME_PACKET_EMBLEM_CREATE_STATISTIC_H_
