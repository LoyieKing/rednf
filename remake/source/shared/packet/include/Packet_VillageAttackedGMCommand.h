//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VILLAGEATTACKEDGMCOMMAND_H
#define PACKET_VILLAGEATTACKEDGMCOMMAND_H

#include "PacketHeader.h"

class Packet_VillageAttackedGMCommand : public PacketHeader {
public:
    unsigned int m_uid;   // +0xa
    unsigned int m_charNo;   // +0xe
    char m_pad[8];           // +0x12
    Packet_VillageAttackedGMCommand() : PacketHeader(0x1779, 0x1a),
        m_fieldA(0), m_charNo(0)
    {

    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_VillageAttackedGMCommand, 0x1a);


#endif //PACKET_VILLAGEATTACKEDGMCOMMAND_H
