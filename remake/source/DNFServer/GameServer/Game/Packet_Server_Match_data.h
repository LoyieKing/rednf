#ifndef DNF_GAME_PACKET_SERVER_MATCH_DATA_H_
#define DNF_GAME_PACKET_SERVER_MATCH_DATA_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PacketCtor_Classes.cpp@86（源码定义复制）
// ORIG W ctor _ZN23Packet_Server_Match_dataC2Ev 0x085df954。

#include "PacketHeader.h"

// 类声明与布局复制自 PacketCtor_Classes.cpp:86-95
//（ctor 语义 PacketHeader(0x1b67,0xd)；三 char 全零）。
class Packet_Server_Match_data : public PacketHeader
{
public:
    Packet_Server_Match_data();
    char m_serverGroup;  // +0xa
    char m_win;          // +0xb
    char m_lose;         // +0xc
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_SERVER_MATCH_DATA_H_
