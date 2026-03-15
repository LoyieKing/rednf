//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H
#define PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H

#include "PacketHeader.h"

class Packet_Event_UpgradeJar_Output_Statistic_GTS : public PacketHeader {
public:
    Packet_Event_UpgradeJar_Output_Statistic_GTS(): PacketHeader(0x106e,0x12) {};
};


#endif //PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H
