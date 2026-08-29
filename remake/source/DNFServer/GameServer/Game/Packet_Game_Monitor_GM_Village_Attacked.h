// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Packet_Game_Monitor_GM_Village_Attacked：ORIG 仅导出弱内联 ctor W @0x0822c3c2；
// LinkFinal2: ctor inline 化 —— ORIG _ZN34Packet_Game_Monitor_GM_Village_AttackedC1Ev objdump
// 逐指令转写：仅 PacketHeader(0x17, 0x234a)，无字段清零。
// 其余成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H_
#define DNF_GAME_PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H_

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Game_Monitor_GM_Village_Attacked : public PacketHeader
{
public:
    Packet_Game_Monitor_GM_Village_Attacked()   // ORIG W @0x0822c3c2
        : PacketHeader(0x17, 0x234a)
    {
    }

    // ORIG oldGmRoutine startva 0x08212890 域字段写（SendTcpPacket 0x17 字节）：
    unsigned char m_flag;          // +0x0a（oldGmRoutine 写 1）
    unsigned char m_pad0b;         // +0x0b
    int m_a;                       // +0x0c
    int m_b;                       // +0x10
    int m_c;                       // +0x14
};

#pragma pack(pop)

#endif  // DNF_GAME_PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H_