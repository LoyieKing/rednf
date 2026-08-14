//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_BOARD_DELETE_H
#define PACKET_GUILD_REQUEST_GUILD_BOARD_DELETE_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Board_Delete : public PacketHeader {
public:
    unsigned int m_boardNo;     // +0xa
    unsigned int m_guildKey;    // +0xe
    unsigned int m_charNo;      // +0x12
    Packet_Guild_Request_Guild_Board_Delete(): PacketHeader(0x2331,0x16) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Request_Guild_Board_Delete, 0x16);


#endif //PACKET_GUILD_REQUEST_GUILD_BOARD_DELETE_H
