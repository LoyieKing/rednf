// ORIG W 0x0846cf34 _ZN26Packet_Request_Add_PvP_BuddyC1Ev（ctor 唯一导出符号，weak 内联）。
// 布局（ORIG ctor 0x0846cf34 + CCommunityServerProxy::SendReqAddPvPBuddy 0x0846cbbc 逐指令实证）：
//   +0x00 PacketHeader(0x1b5c, 0x31)  （+2 packetSize=0x31=sizeof）
//   +0x0a unsigned int m_accId        （ctor 清零；Send 填 param_1）
//   +0x0e unsigned int m_characNo     （ctor 清零；Send 填 param_2）
//   +0x12 char m_flag                 （ctor 清零；Send 填 param_3）
//   +0x13 char m_name[0x1e]           （ctor memset 30B；Send memcpy(param_4, param_5)）
//   sizeof = 0x31。
#ifndef DNF_GAME_PACKET_REQUEST_ADD_PVP_BUDDY_H_
#define DNF_GAME_PACKET_REQUEST_ADD_PVP_BUDDY_H_

#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Request_Add_PvP_Buddy : public PacketHeader
{
public:
    Packet_Request_Add_PvP_Buddy()   // ORIG W 0x0846cf34
        : PacketHeader(0x1b5c, 0x31)
    {
        m_accId = 0;
        m_characNo = 0;
        m_flag = 0;
        memset(m_name, 0, sizeof(m_name));
    }

    unsigned int m_accId;      // +0x0a
    unsigned int m_characNo;   // +0x0e
    char m_flag;               // +0x12
    char m_name[0x1e];         // +0x13（30 字节）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Request_Add_PvP_Buddy) == 0x31,
              "Packet_Request_Add_PvP_Buddy layout");

#endif  // DNF_GAME_PACKET_REQUEST_ADD_PVP_BUDDY_H_
