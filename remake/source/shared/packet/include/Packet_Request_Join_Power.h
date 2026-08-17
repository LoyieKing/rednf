//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_JOIN_POWER_H
#define PACKET_REQUEST_JOIN_POWER_H

#include "PacketHeader.h"

class Packet_Request_Join_Power : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa（SendJoinPower param_1；接收端 FindUser_CharNo）
    unsigned int m_guildKey;    // +0xe（SendJoinPower param_2；接收端 FindGuild）
    unsigned char m_powerSide;  // +0x12（SendJoinPower param_3；接收端：==1 → side 3，否则 side 4）
    Packet_Request_Join_Power() : PacketHeader(0x6a8, 0x13){};
} __attribute__((packed));

#endif  // PACKET_REQUEST_JOIN_POWER_H
