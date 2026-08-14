//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_LOGOUT_H
#define PACKET_MONITOR_UDP_LOGOUT_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_Logout : public PacketHeader {
public:
    unsigned int m_dbid;            // +0xa  FindUser
    unsigned char m_channel;        // +0xe
    int m_cache0;                   // +0xf  CACHE_CHARACTER_TYPE.m_field0
    char m_pad13[4];                // +0x13
    unsigned char m_foc;            // +0x17 Force of Connection
    unsigned int m_memberKey;       // +0x18
    char m_pad1c[0x1e];             // +0x1c
    unsigned char m_cacheFlag;      // +0x3a
    Packet_Monitor_UDP_Logout() : PacketHeader(0x3e9, 0x3b){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_UDP_Logout, 0x3b);

#endif  // PACKET_MONITOR_UDP_LOGOUT_H
