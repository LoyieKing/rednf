//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_LOGOUT_H
#define PACKET_HADES_LOGOUT_H

#include "PacketHeader.h"

class Packet_Hades_Logout : public PacketHeader {
public:
    unsigned int m_accId;   // +0x0a（Send_Logout @84b92ea：CUser::get_acc_id）
    Packet_Hades_Logout() : PacketHeader(0xfd3, 0xe),
        m_accId(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Hades_Logout, 0xe);

#endif  // PACKET_HADES_LOGOUT_H
