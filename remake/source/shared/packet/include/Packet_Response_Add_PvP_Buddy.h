//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_RESPONSE_ADD_PVP_BUDDY_H
#define PACKET_RESPONSE_ADD_PVP_BUDDY_H

#include "PacketHeader.h"
#include <string.h>

#pragma pack(push, 1)
class Packet_Response_Add_PvP_Buddy : public PacketHeader {
public:
    int m_id;
    int charac_no;
    char server_id;
    char buddy_n_user_id_what[30];
    Packet_Response_Add_PvP_Buddy() : PacketHeader(0x1b5e, 0x31),
        m_id(0), charac_no(0), server_id(0)
    {
    memset(buddy_n_user_id_what, 0, sizeof(buddy_n_user_id_what));
    };
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Response_Add_PvP_Buddy, 0x31);

#endif  // PACKET_RESPONSE_ADD_PVP_BUDDY_H
