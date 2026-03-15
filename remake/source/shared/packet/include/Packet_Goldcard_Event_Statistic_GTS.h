//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H
#define PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H

#include "PacketHeader.h"

class Packet_Goldcard_Event_Statistic_GTS : public PacketHeader {
public:
    Packet_Goldcard_Event_Statistic_GTS(): PacketHeader(0x1f46,0x13) {};
};


#endif //PACKET_GOLDCARD_EVENT_STATISTIC_GTS_H
