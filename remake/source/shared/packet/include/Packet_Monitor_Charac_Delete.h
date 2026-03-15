//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CHARAC_DELETE_H
#define PACKET_MONITOR_CHARAC_DELETE_H

#include "PacketHeader.h"

class Packet_Monitor_Charac_Delete : public PacketHeader {
public:
    Packet_Monitor_Charac_Delete() : PacketHeader(0x4c2, 0x16){};
};

#endif  // PACKET_MONITOR_CHARAC_DELETE_H
