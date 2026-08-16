//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H
#define PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H

#include "PacketHeader.h"

class Packet_GTM_OntimeEvent_RequestReward : public PacketHeader {
public:
    int m_fieldA;  // +a
    unsigned int m_fieldB;  // +e
    int m_fieldC;  // +12

    Packet_GTM_OntimeEvent_RequestReward() : PacketHeader(0x17c3, 0x16),
        m_fieldA(0), m_fieldB(0xffffffffu), m_fieldC(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_GTM_OntimeEvent_RequestReward, 0x16);


#endif //PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H
