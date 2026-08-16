//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CHARAC_DELETE_H
#define PACKET_MONITOR_CHARAC_DELETE_H

#include "PacketHeader.h"

class Packet_Monitor_Charac_Delete : public PacketHeader {
public:
    unsigned int m_accId;      // +0xa
    unsigned int m_charNo;      // +0xe
    union {
        unsigned int m_field12;     // +0x12
        unsigned int m_guildKey;    // +0x12  Guild OnCharacterDelete
    };
    Packet_Monitor_Charac_Delete() : PacketHeader(0x4c2, 0x16){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Charac_Delete, 0x16);

#endif  // PACKET_MONITOR_CHARAC_DELETE_H
