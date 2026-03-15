//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_VILLAGEATTACKEDGMCOMMAND_H
#define PACKET_VILLAGEATTACKEDGMCOMMAND_H

#include "PacketHeader.h"

class Packet_VillageAttackedGMCommand : public PacketHeader {
public:
    Packet_VillageAttackedGMCommand(): PacketHeader(0x1779,0x1a) {};
};


#endif //PACKET_VILLAGEATTACKEDGMCOMMAND_H
