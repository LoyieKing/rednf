//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PCROOM_RESPONSE_H
#define PACKET_PCROOM_RESPONSE_H

#include "PacketHeader.h"

class Packet_Pcroom_Response : public PacketHeader {
public:
    Packet_Pcroom_Response() : PacketHeader(0xfaf, 0x1f){};
};

#endif  // PACKET_PCROOM_RESPONSE_H
