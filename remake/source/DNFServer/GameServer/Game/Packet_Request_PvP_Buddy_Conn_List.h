// HdrCreate3 壳；EventBattleLink 批次按 ORIG ctor 0x0846cefc 补全字段：
// PacketHeader(0x1b5a, 0x12) + 两个 int @+0x0a/+0x0e（ctor 清零；
// SendPvPBuddyList 0x0846cb80 填 accId/characNo 后 SendTcpPacket(0x12)）。
#ifndef DNF_GAME_PACKET_REQUEST_PVP_BUDDY_CONN_LIST_H_
#define DNF_GAME_PACKET_REQUEST_PVP_BUDDY_CONN_LIST_H_

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Request_PvP_Buddy_Conn_List : public PacketHeader
{
public:
    Packet_Request_PvP_Buddy_Conn_List();  // ORIG W 0x0846cefc

    unsigned int m_accId;      // +0x0a（ctor 清零；SendPvPBuddyList 填）
    unsigned int m_characNo;   // +0x0e（ctor 清零；SendPvPBuddyList 填）
};

#pragma pack(pop)

static_assert(sizeof(Packet_Request_PvP_Buddy_Conn_List) == 0x12,
              "Packet_Request_PvP_Buddy_Conn_List layout");

#endif  // DNF_GAME_PACKET_REQUEST_PVP_BUDDY_CONN_LIST_H_
