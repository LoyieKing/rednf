// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x085bee52 (C1 W) + 提取自 PartyClosureC2.cpp:210 定义段（原 cpp 内定义，未改 cpp）。
#ifndef DNF_GAME_PACKET_BLOOD_DUNGEON_STATISTIC_H_
#define DNF_GAME_PACKET_BLOOD_DUNGEON_STATISTIC_H_

#include "PacketHeader.h"

// 注：+0xa..+0xd 在 ORIG 中为未初始化栈值（见 PartyClosureC2.cpp:39 注释）。
class Packet_Blood_dungeon_statistic : public PacketHeader
{
public:
    Packet_Blood_dungeon_statistic();
    char m_padA[4];                  // +0x0a（ORIG 栈上未初始化）
    int m_level;                     // +0x0e
    unsigned char m_one;             // +0x12（恒 1）
    unsigned char m_success;         // +0x13
} __attribute__((packed));           // ctor: PacketHeader(0x1036, 0x14)

#endif  // DNF_GAME_PACKET_BLOOD_DUNGEON_STATISTIC_H_
