//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H
#define PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H

#include "PacketHeader.h"

class Packet_GTM_OntimeEvent_RequestReward : public PacketHeader {
public:
    Packet_GTM_OntimeEvent_RequestReward(): PacketHeader(0x17c3,0x16) {};
};


#endif //PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H
