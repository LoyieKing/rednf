//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SERVER_MATCH_DATA_H
#define PACKET_SERVER_MATCH_DATA_H

#include "PacketHeader.h"

class Packet_Server_Match_data : public PacketHeader {
public:
    Packet_Server_Match_data() : PacketHeader(0x1b67, 0xd),
        m_fieldA(0), m_fieldB(0), m_fieldC(0)
    {

    };
    char m_fieldA;   // +0xa（ORIG 还原：AddServerMatchData 直接成员位移寻址）
    char m_fieldB;   // +0xb
    char m_fieldC;   // +0xc
} __attribute__((packed));

#endif  // PACKET_SERVER_MATCH_DATA_H
