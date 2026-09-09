// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x08110bdc (C1 W) + 提取自 W8_EventClassifyB.cpp:94 定义段（原 cpp 内定义，未改 cpp）。
#ifndef DNF_GAME_PACKET_EMBLEM_CREATE_STATISTIC_H_
#define DNF_GAME_PACKET_EMBLEM_CREATE_STATISTIC_H_

#include <cstring>

#include "PacketHeader.h"

// ctor W 0x08110bdc：header(0x17a4,0x36) 后清零 dword@+0xa。ORIG 循环内写入
// 地址为 -0x40+4i(grade) 与 -0x2c+4i(value)（SendCreateEmblemStatistic
// 0x0810f742 实证；DispLinkA1.cpp 匿名镜像注释同）。以槽数组建模：
// slots[0] 尾写条数、slots[i+1]=grade_i、slots[i+6]=value_i（b28c-L0 修正）。
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

    // [b28d-solo-DispLinkA1 P311/P246] ORIG 写入点为两个独立具名数组成员
    // （grade@+0xe+4i、value@+0x22+4i，SendCreateEmblemInfo 0x08218ae7/0x08218af4
    // 实证）：单条 `mov %eax,-0x4c(%ebp,%ebx,4)`（disp 折叠）只在成员数组
    // offset>0 时出现；`m_slots[count+1]` 恒折 lea。union 双视图零布局影响。
    union
    {
        int m_slots[11];         // +0x0a..（0x2c 字节 = 11×int）
        struct
        {
            int m_count;         // +0x00（= m_slots[0]）
            int m_grades[5];     // +0x04（= m_slots[1..5]）
            int m_values[5];     // +0x18（= m_slots[6..10]）
        };
    };
} __attribute__((packed));       // sizeof == 0x36

#endif  // DNF_GAME_PACKET_EMBLEM_CREATE_STATISTIC_H_
