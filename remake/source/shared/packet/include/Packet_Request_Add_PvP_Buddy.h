//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_ADD_PVP_BUDDY_H
#define PACKET_REQUEST_ADD_PVP_BUDDY_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Request_Add_PvP_Buddy : public PacketHeader {
public:
    int m_id;                         // offset 10
    int charac_no;                    // offset 14
    char server_id;                   // offset 0x12
    char buddy_n_user_id_what[0x1e];  // offset 0x13

    Packet_Request_Add_PvP_Buddy()
        : PacketHeader(0x1b5c, 0x31){};
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Request_Add_PvP_Buddy, 0x31);

#endif  // PACKET_REQUEST_ADD_PVP_BUDDY_H
