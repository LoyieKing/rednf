//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_LOGIN_H
#define PACKET_MONITOR_UDP_LOGIN_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_Login : public PacketHeader {
public:
    union {
        int m_channelId;            // +0xa  Monitor CreateUser 频道参数
        int m_guildKey;             // +0xa  Guild OnLogin
    };
    union {
        unsigned int m_dbid;        // +0xe
        unsigned int m_serverNo;    // +0xe  Guild OnLogin
    };
    unsigned char m_channel;    // +0x12 FindGameServer
    char m_pad13[0x10];         // +0x13
    unsigned char m_sex;        // +0x23
    unsigned int m_cache0;      // +0x24
    unsigned int m_cache4;      // +0x28
    char m_ssn[7];              // +0x2c
    Packet_Monitor_UDP_Login() : PacketHeader(0x3e8, 0x33){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_UDP_Login, 0x33);

#endif  // PACKET_MONITOR_UDP_LOGIN_H
