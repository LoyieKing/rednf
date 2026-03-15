//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_SERVER_MESSAGE_INFO_H
#define PACKET_MONITOR_SERVER_MESSAGE_INFO_H

#include "PacketHeader.h"

class Packet_Monitor_Server_Message_Info : public PacketHeader {
public:
    Packet_Monitor_Server_Message_Info(): PacketHeader(0xfae,0x5f) {};
};


#endif //PACKET_MONITOR_SERVER_MESSAGE_INFO_H
