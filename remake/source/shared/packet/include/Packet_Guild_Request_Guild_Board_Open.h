//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_REQUEST_GUILD_BOARD_OPEN_H
#define PACKET_GUILD_REQUEST_GUILD_BOARD_OPEN_H

#include "PacketHeader.h"

class Packet_Guild_Request_Guild_Board_Open : public PacketHeader {
public:
    unsigned char m_boardType;  // +0xa
    unsigned int m_guildKey;    // +0xb
    unsigned int m_charNo;      // +0xf
    Packet_Guild_Request_Guild_Board_Open(): PacketHeader(0x2329,0x13) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Request_Guild_Board_Open, 0x13);


#endif //PACKET_GUILD_REQUEST_GUILD_BOARD_OPEN_H
