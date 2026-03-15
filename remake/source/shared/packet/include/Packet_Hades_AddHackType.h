//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_ADDHACKTYPE_H
#define PACKET_HADES_ADDHACKTYPE_H

#include "PacketHeader.h"

class Packet_Hades_AddHackType : public PacketHeader {
public:
    Packet_Hades_AddHackType() : PacketHeader(0xfd7, 0x1a){};
};

#endif  // PACKET_HADES_ADDHACKTYPE_H
