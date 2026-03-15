//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_LOGIN_H
#define PACKET_MONITOR_UDP_LOGIN_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_Login : public PacketHeader {
public:
    Packet_Monitor_UDP_Login() : PacketHeader(0x3e8, 0x33){};
};

#endif  // PACKET_MONITOR_UDP_LOGIN_H
