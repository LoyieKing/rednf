//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DOUBLECHECK_UDP_LOGIN_H
#define PACKET_DOUBLECHECK_UDP_LOGIN_H

#include "PacketHeader.h"

class Packet_DoubleCheck_UDP_Login : public PacketHeader {
public:
    unsigned int m_userID;  // +a（SendLogin 参数1＝用户 DB id，同族 Packet_DoubleCheck_UDP_Logout 同名）
    unsigned char m_channel;  // +e（SendLogin 参数2＝CEnvironment::get_channel_no）
    unsigned char m_group;  // +f（SendLogin 参数3＝CEnvironment+0x378 组号；同族 Logout 同名）

    Packet_DoubleCheck_UDP_Login() : PacketHeader(0xbb8, 0x10),
        m_userID(0), m_channel(255), m_group(255)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_DoubleCheck_UDP_Login, 0x10);

#endif  // PACKET_DOUBLECHECK_UDP_LOGIN_H
