#ifndef DNF_GAME_PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H_
#define DNF_GAME_PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PacketCtor_Classes.cpp@121（源码定义复制）
// ORIG W ctor _ZN38Packet_Double_Check_Server_UDP_HeartBeatC2Ev 0x0846d602。
// 冲突说明：CDoubleConnCheckServerProxy.cpp@15 另有一份无继承的本地镜像
//（同一 ORIG 符号，+0x00..+0x0b 字节布局一致）；本头采用 PacketCtor_Classes.cpp
// 的继承 PacketHeader 完整定义作为权威声明。

#include "PacketHeader.h"

// 类声明与布局复制自 PacketCtor_Classes.cpp:121-129
//（ctor 语义 PacketHeader(0xbba,0xc)；m_channel=255）。
class Packet_Double_Check_Server_UDP_HeartBeat : public PacketHeader
{
public:
    Packet_Double_Check_Server_UDP_HeartBeat();
    unsigned char m_channel;  // +0xa
    unsigned char m_group;    // +0xb
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H_
