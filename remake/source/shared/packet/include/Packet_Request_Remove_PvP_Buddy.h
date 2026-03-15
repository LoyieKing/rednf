//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_REMOVE_PVP_BUDDY_H
#define PACKET_REQUEST_REMOVE_PVP_BUDDY_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Request_Remove_PvP_Buddy : public PacketHeader {
public:
    int m_id;
    int charac_no;
    char server_id;
    char buddy_n_user_id_what[30];

    Packet_Request_Remove_PvP_Buddy() : PacketHeader(0x1b60, 0x31){};
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Request_Remove_PvP_Buddy, 0x31);

#endif  // PACKET_REQUEST_REMOVE_PVP_BUDDY_H
