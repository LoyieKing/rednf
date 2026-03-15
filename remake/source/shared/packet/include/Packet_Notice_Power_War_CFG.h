//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_POWER_WAR_CFG_H
#define PACKET_NOTICE_POWER_WAR_CFG_H

#include "PacketHeader.h"

class Packet_Notice_Power_War_CFG : public PacketHeader {
public:
    Packet_Notice_Power_War_CFG() : PacketHeader(0x6aa, 0x1c){};
};

#endif  // PACKET_NOTICE_POWER_WAR_CFG_H
