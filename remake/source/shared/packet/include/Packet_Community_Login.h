//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COMMUNITY_LOGIN_H
#define PACKET_COMMUNITY_LOGIN_H

#include "PacketHeader.h"
#include <string.h>
#include "STGameUserInfo.h"
#include "STPvPBuddyDBInfo.h"

#pragma pack(push, 1)
class Packet_Community_Login : public PacketHeader {
public:
    int m_id;
    STGameUserInfo gameUserInfo;
    int buddyCount;
    STPvPBuddyDBInfo buddies[0x20];
    Packet_Community_Login() : PacketHeader(0x1b58, 0x5a2)
    {
    *(int*)((char*)this + 0xa) = 0;
    *(int*)((char*)this + 0x3e) = 0;
    memset((char*)this + 0xe, 0, 0x30);
    memset((char*)this + 0x42, 0, 0x560);
    };
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Community_Login, 0x5a2);

#endif  // PACKET_COMMUNITY_LOGIN_H
