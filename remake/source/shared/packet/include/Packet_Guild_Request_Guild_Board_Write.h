//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H
#define PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Board_Write : public PacketHeader {
public:
    unsigned int m_boardNo;     // +0xa
    unsigned int m_guildKey;    // +0xe
    unsigned int m_charNo;      // +0x12
    unsigned char m_field16;    // +0x16
    unsigned char m_field17;    // +0x17
    char m_info[0x78];          // +0x18
    Packet_Guild_Request_Guild_Board_Write(): PacketHeader(0x232d,0x90) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Request_Guild_Board_Write, 0x90);


#endif //PACKET_GUILD_REQUEST_GUILD_BOARD_WRITE_H
