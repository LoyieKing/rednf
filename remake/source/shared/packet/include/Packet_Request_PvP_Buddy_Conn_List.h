//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_PVP_BUDDY_CONN_LIST_H
#define PACKET_REQUEST_PVP_BUDDY_CONN_LIST_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Request_PvP_Buddy_Conn_List : public PacketHeader {
public:
    int m_id;       // offset 0x0a
    int charac_no;  // offset 0x0e

    Packet_Request_PvP_Buddy_Conn_List()
        : PacketHeader(0x1b5a, 0x12){};
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Request_PvP_Buddy_Conn_List, 0x12);

#endif  // PACKET_REQUEST_PVP_BUDDY_CONN_LIST_H
