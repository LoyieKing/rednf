//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_POWER_WAR_CFG_H
#define PACKET_NOTICE_POWER_WAR_CFG_H

#include "PacketHeader.h"

class Packet_Notice_Power_War_CFG : public PacketHeader {
public:
    unsigned short m_endKillPoint;   // +0xa
    int m_basic;                     // +0xc
    int m_first;                     // +0x10
    int m_decrease;                  // +0x14
    int m_ranking;                   // +0x18
    Packet_Notice_Power_War_CFG() : PacketHeader(0x6aa, 0x1c){};
} __attribute__((packed));

#endif  // PACKET_NOTICE_POWER_WAR_CFG_H
