//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_UDP_HEARTBEAT_H
#define PACKET_HADES_UDP_HEARTBEAT_H

#include "PacketHeader.h"

class Packet_Hades_Udp_HeartBeat : public PacketHeader {
public:
    Packet_Hades_Udp_HeartBeat() : PacketHeader(0xfd8, 0xe){};
};

#endif  // PACKET_HADES_UDP_HEARTBEAT_H
