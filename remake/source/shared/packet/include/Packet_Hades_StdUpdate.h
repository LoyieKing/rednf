//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_STDUPDATE_H
#define PACKET_HADES_STDUPDATE_H

#include "PacketHeader.h"

class Packet_Hades_StdUpdate : public PacketHeader {
public:
    Packet_Hades_StdUpdate() : PacketHeader(0xfd9, 0x12){};
};

#endif  // PACKET_HADES_STDUPDATE_H
