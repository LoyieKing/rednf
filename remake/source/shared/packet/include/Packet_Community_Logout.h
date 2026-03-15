//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COMMUNITY_LOGOUT_H
#define PACKET_COMMUNITY_LOGOUT_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Community_Logout : public PacketHeader {
public:
    int m_id;  // offset 0xa
    int what_0xe;  // offset 0xe

    Packet_Community_Logout()
        : PacketHeader(0x1b59, 0x12){};
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Community_Logout, 0x12);

#endif  // PACKET_COMMUNITY_LOGOUT_H
