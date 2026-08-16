//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DOUBLECHECK_UDP_LOGOUT_H
#define PACKET_DOUBLECHECK_UDP_LOGOUT_H

#include "PacketHeader.h"

class Packet_DoubleCheck_UDP_Logout : public PacketHeader {
public:
    int m_fieldA;  // +a
    unsigned char m_fieldB;  // +e
    unsigned char m_fieldC;  // +f

    Packet_DoubleCheck_UDP_Logout() : PacketHeader(0xbb9, 0x10),
        m_fieldA(0), m_fieldB(255), m_fieldC(255)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_DoubleCheck_UDP_Logout, 0x10);

#endif  // PACKET_DOUBLECHECK_UDP_LOGOUT_H
