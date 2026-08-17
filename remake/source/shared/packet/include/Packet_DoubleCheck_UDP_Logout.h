//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DOUBLECHECK_UDP_LOGOUT_H
#define PACKET_DOUBLECHECK_UDP_LOGOUT_H

#include "PacketHeader.h"

class Packet_DoubleCheck_UDP_Logout : public PacketHeader {
public:
    int m_accId;  // +a（SendLogout/SendLogin param_1 = DB ID/acc id，Inter_LoginInfo 中 set_acc_info 同源）
    unsigned char m_channel;  // +e（SendLogin 调用方: CEnvironment::get_channel_no）
    unsigned char m_serverGroup;  // +f（SendLogin 调用方: env+0x378，与 GetServerGroup 同义使用）

    Packet_DoubleCheck_UDP_Logout() : PacketHeader(0xbb9, 0x10),
        m_accId(0), m_channel(255), m_serverGroup(255)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_DoubleCheck_UDP_Logout, 0x10);

#endif  // PACKET_DOUBLECHECK_UDP_LOGOUT_H
