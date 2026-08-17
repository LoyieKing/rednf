//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_LOGIN_H
#define PACKET_HADES_LOGIN_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Hades_Login : public PacketHeader {
public:
    unsigned int m_accId;   // +0x0a（Send_Login @84b9200：CUser::get_acc_id）
    char m_ip[16];          // +0x0e（CHades::GetIP @84b9774：登录时抓取的 peer IP，
                            //      GetPeerIP2(...,4) 恒失败回退 CUser+0x8e418，strcpy 16B）
    char m_mac[65];         // +0x1e（strncpy(..., CNetwork::get_mac_addr, 0x41)，源 CNetwork+0x6ee2c）
    unsigned char m_channel;// +0x5f（CEnvironment::get_channel_no，int→byte 截断）
    unsigned int m_uid;     // +0x60（CUser::GetUID，CUser+0x704a4/0x704a6 两 u16 拼装）
    Packet_Hades_Login() : PacketHeader(0xfd2, 0x64),
        m_accId(0), m_channel(0), m_uid(0)
    {
        memset(m_ip, 0, sizeof(m_ip));
        memset(m_mac, 0, sizeof(m_mac));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Hades_Login, 0x64);

#endif  // PACKET_HADES_LOGIN_H
