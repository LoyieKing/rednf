//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H
#define PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H

#include "PacketHeader.h"

class Packet_Double_Check_Server_UDP_HeartBeat : public PacketHeader {
public:
    Packet_Double_Check_Server_UDP_HeartBeat(): PacketHeader(0xbba,0xc) {};
};


#endif //PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H
