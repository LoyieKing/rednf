//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GAME_SERVER_REGIST_H
#define PACKET_GAME_SERVER_REGIST_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Game_Server_Regist : public PacketHeader {
public:
    unsigned char m_type;       // +0xa
    unsigned char m_group;      // +0xb
    unsigned char m_channel;    // +0xc
    char m_name[0x10];          // +0xd
    unsigned short m_port;      // +0x1d
    Packet_Game_Server_Regist() : PacketHeader(0x1f41, 0x1f),
        m_type(0), m_group(0), m_channel(0), m_port(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Game_Server_Regist, 0x1f);

#endif  // PACKET_GAME_SERVER_REGIST_H
