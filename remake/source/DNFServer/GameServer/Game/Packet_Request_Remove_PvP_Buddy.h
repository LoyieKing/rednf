// ORIG W 0x0846cfec _ZN31Packet_Request_Remove_PvP_BuddyC1Ev（weak 内联）。
// 布局（ORIG ctor 0x0846cfec + CCommunityServerProxy::SendRemovePvPBuddy 0x0846cc88 实证）：
//   +0x00 PacketHeader(0x1b60, 0x31)（+2 packetSize=0x31=sizeof）
//   +0x0a unsigned int m_accId / +0x0e unsigned int m_characNo /
//   +0x12 char m_flag / +0x13 char m_name[0x1e]；sizeof = 0x31。
#ifndef DNF_GAME_PACKET_REQUEST_REMOVE_PVP_BUDDY_H_
#define DNF_GAME_PACKET_REQUEST_REMOVE_PVP_BUDDY_H_

#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Request_Remove_PvP_Buddy : public PacketHeader
{
public:
    Packet_Request_Remove_PvP_Buddy()   // ORIG W 0x0846cfec
        : PacketHeader(0x1b60, 0x31)
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

static_assert(sizeof(Packet_Request_Remove_PvP_Buddy) == 0x31,
              "Packet_Request_Remove_PvP_Buddy layout");

#endif  // DNF_GAME_PACKET_REQUEST_REMOVE_PVP_BUDDY_H_
