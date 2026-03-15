//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_EXCHANGE_SERVER_INFO_H
#define PACKET_EXCHANGE_SERVER_INFO_H

#include "PacketHeader.h"

class Packet_Exchange_Server_Info : public PacketHeader {
public:
    Packet_Exchange_Server_Info() : PacketHeader(0x9de, 0x14){};
};

#endif  // PACKET_EXCHANGE_SERVER_INFO_H
