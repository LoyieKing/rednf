//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_LOGOUT_H
#define PACKET_MONITOR_UDP_LOGOUT_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_Logout : public PacketHeader {
public:
    Packet_Monitor_UDP_Logout() : PacketHeader(0x3e9, 0x3b){};
};

#endif  // PACKET_MONITOR_UDP_LOGOUT_H
