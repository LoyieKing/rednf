//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_LOGOUT_COMPLETE_H
#define PACKET_MONITOR_UDP_LOGOUT_COMPLETE_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_Logout_Complete : public PacketHeader {
public:
    Packet_Monitor_UDP_Logout_Complete(): PacketHeader(0x3f4,0xf) {};
};


#endif //PACKET_MONITOR_UDP_LOGOUT_COMPLETE_H
