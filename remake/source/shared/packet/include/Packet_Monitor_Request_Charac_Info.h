//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_REQUEST_CHARAC_INFO_H
#define PACKET_MONITOR_REQUEST_CHARAC_INFO_H

#include "PacketHeader.h"

class Packet_Monitor_Request_Charac_Info : public PacketHeader {
public:
    Packet_Monitor_Request_Charac_Info(): PacketHeader(0x27de,0x23) {};
};


#endif //PACKET_MONITOR_REQUEST_CHARAC_INFO_H
