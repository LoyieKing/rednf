#ifndef DNF_GAME_PACKET_Packet_Monitor_Set_Sub_Guild_Master_H_
#define DNF_GAME_PACKET_Packet_Monitor_Set_Sub_Guild_Master_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822b978
// _ZN35Packet_Monitor_Set_Sub_Guild_MasterC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x31。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Monitor_Set_Sub_Guild_Master : public PacketHeader
{
public:
    Packet_Monitor_Set_Sub_Guild_Master();   // ORIG W @0x0822b978

    // [b28c-34] 空壳 m_data 改具名字段：ORIG ctor 0x0822b99c/0x0822b9a6 为
    // `movl $0x0,0xa(%eax)` / `movl $0x0,0xe(%eax)` disp 直写（成员赋值）；
    // 裸偏移形令 4.4 恒发 add 链。布局与 shared/packet/include 同名头一致。
    unsigned int m_charNo;      // +0x0a
    unsigned int m_guildKey;    // +0x0e
    char m_name[0x1e];          // +0x12..0x2f
    unsigned char m_grade;      // +0x30
};

#pragma pack(pop)

static_assert(sizeof(Packet_Monitor_Set_Sub_Guild_Master) == 0x31, "Packet_Monitor_Set_Sub_Guild_Master layout");

#endif  // DNF_GAME_PACKET_Packet_Monitor_Set_Sub_Guild_Master_H_
