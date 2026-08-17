//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COMMUNITY_LOGOUT_H
#define PACKET_COMMUNITY_LOGOUT_H

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Community_Logout : public PacketHeader {
public:
    int m_id;        // offset 0xa（CCommunityServerProxy::SendLogout param_1；Community find_user(m_id)）
    int m_characNo;  // offset 0xe（SendLogout param_2；Community OnLogout:
                     //         user->get_user_info()->charac_no == packet->m_characNo）
    Packet_Community_Logout() : PacketHeader(0x1b59, 0x12),
        m_id(0), m_characNo(0)
    {

    };
} __attribute__((packed));
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Community_Logout, 0x12);

#endif  // PACKET_COMMUNITY_LOGOUT_H
