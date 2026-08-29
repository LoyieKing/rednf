#ifndef DNF_GAME_PACKET_CHANNELTYPE_H_
#define DNF_GAME_PACKET_CHANNELTYPE_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PacketCtor_Classes.cpp@17（源码定义复制）
// ORIG W ctor _ZN18Packet_ChannelTypeC2Ev 0x082a4088（C1/C2 同址弱别名）。

#include "PacketHeader.h"

// 类声明与布局复制自 PacketCtor_Classes.cpp:17-25（ctor 语义 PacketHeader(0xfa4,0x12)）。
class Packet_ChannelType : public PacketHeader
{
public:
    Packet_ChannelType();
    unsigned int m_channel;  // +0xa
    int m_type;              // +0xe
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_CHANNELTYPE_H_
