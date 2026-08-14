//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GAME_SERVER_REGIST_H
#define PACKET_GAME_SERVER_REGIST_H

#include "PacketHeader.h"

class Packet_Game_Server_Regist : public PacketHeader {
public:
    unsigned char m_type;       // +0xa
    unsigned char m_group;      // +0xb
    unsigned char m_channel;    // +0xc
    char m_name[0x10];          // +0xd
    unsigned short m_port;      // +0x1d
    Packet_Game_Server_Regist() : PacketHeader(0x1f41, 0x1f){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Game_Server_Regist, 0x1f);

#endif  // PACKET_GAME_SERVER_REGIST_H
