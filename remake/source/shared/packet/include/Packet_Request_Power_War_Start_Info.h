//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_POWER_WAR_START_INFO_H
#define PACKET_REQUEST_POWER_WAR_START_INFO_H

#include "PacketHeader.h"

class Packet_Request_Power_War_Start_Info : public PacketHeader {
public:
    Packet_Request_Power_War_Start_Info(): PacketHeader(0x6b1,0xe) {};
};


#endif //PACKET_REQUEST_POWER_WAR_START_INFO_H
