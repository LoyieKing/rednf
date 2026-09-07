// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x0822c7c0 (C1 W), nm-only.
// LinkFinal2: ctor inline 化 —— ORIG _ZN20Packet_CollectItemsGmC1Ev W @0x0822c7c0 objdump 逐指令转写：
// PacketHeader(0x27eb, 0x17)；+0x0a byte=0；+0x0b/+0x0f/+0x13 各 4B 清零。
#ifndef DNF_GAME_PACKET_COLLECTITEMSGM_H_
#define DNF_GAME_PACKET_COLLECTITEMSGM_H_

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_CollectItemsGm : public PacketHeader
{
public:
    Packet_CollectItemsGm()   // ORIG W @0x0822c7c0
        : PacketHeader(0x27eb, 0x17)   // packetId=0x27eb、size=0x17（ORIG C1 0x4(%esp)=0x27eb 实证）
    {
        m_pad0a = 0;
        m_a = 0;   // +0x0b
        m_b = 0;   // +0x0f
        m_c = 0;   // +0x13
    }

    // ORIG oldGmRoutine timegate 0x082121bd 域字段写（SendTcpPacket 0x17 字节）：
    unsigned char m_pad0a;         // +0x0a
    int m_a;                       // +0x0b（atoi(args[1])）
    int m_b;                       // +0x0f（atoi(args[2])）
    int m_c;                       // +0x13（ORIG ctor 清零区）
};

#pragma pack(pop)

static_assert(sizeof(Packet_CollectItemsGm) == 0x17, "Packet_CollectItemsGm layout");

#endif  // DNF_GAME_PACKET_COLLECTITEMSGM_H_