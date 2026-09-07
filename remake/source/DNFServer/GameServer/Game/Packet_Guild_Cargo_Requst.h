#ifndef DNF_GAME_PACKET_Packet_Guild_Cargo_Requst_H_
#define DNF_GAME_PACKET_Packet_Guild_Cargo_Requst_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822be52
// _ZN25Packet_Guild_Cargo_RequstC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + 两 int 载荷，sizeof=0x12。
// 字段化（b23·§8.3）：char m_data[8] 按 ORIG ctor 切片拆具名字段——
//   0x0822be76 movl $0x0,0xa(%eax) / 0x0822be80 movl $0x0,0xe(%eax)
//   ⇒ +0x0a、+0x0e 各一 int 且 ctor 置零（Dispatchers.cpp:3119 以
//   getCurCharacNo() 填 +0x0a ⇒ 命名 m_characNo；+0x0e 无语义证据，
//   保留偏移名）。
#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Guild_Cargo_Requst : public PacketHeader
{
public:
    Packet_Guild_Cargo_Requst();   // ORIG W @0x0822be52

    int m_characNo;   // +0x0a（ORIG ctor 置零；Dispatchers 填 characNo）
    int m_field0e;    // +0x0e（ORIG ctor 置零）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Guild_Cargo_Requst) == 0x12, "Packet_Guild_Cargo_Requst layout");

#endif  // DNF_GAME_PACKET_Packet_Guild_Cargo_Requst_H_
