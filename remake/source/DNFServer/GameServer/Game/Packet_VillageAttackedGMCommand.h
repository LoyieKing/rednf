#ifndef DNF_GAME_PACKET_VILLAGEATTACKEDGMCOMMAND_H_
#define DNF_GAME_PACKET_VILLAGEATTACKEDGMCOMMAND_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + GmCmds_G.cpp@105（源码定义复制）
// ORIG W ctor _ZN29Packet_VillageAttackedGMCommandC2Ev 0x0822c088。

#include "PacketHeader.h"

// 类声明与布局复制自 GmCmds_G.cpp:105-118
//（ctor 语义 PacketHeader(0x1779,0x1a)；清 +0x0a/+0x0e）。
struct Packet_VillageAttackedGMCommand : public PacketHeader
{
    Packet_VillageAttackedGMCommand()
        : PacketHeader(0x1779, 0x1a), m_uid(0), m_characNo(0)  // init-list → ORIG 直址 movl $0,0xa/0xe
    {
    }

    unsigned int m_uid;       // +0x0a
    unsigned int m_characNo;  // +0x0e
    unsigned int m_vill;      // +0x12（parse 结果）
    unsigned int m_field16;   // +0x16（execute 置 0）
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_VILLAGEATTACKEDGMCOMMAND_H_
