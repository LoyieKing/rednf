//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H
#define PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H

#include "PacketHeader.h"

class Packet_Double_Check_Server_UDP_HeartBeat : public PacketHeader {
public:
    unsigned char m_fieldA;  // +a
    char m_padb[0x1];  // +b..+b（ctor 未触碰/布局待定）

    Packet_Double_Check_Server_UDP_HeartBeat() : PacketHeader(0xbba, 0xc),
        m_fieldA(255)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Double_Check_Server_UDP_HeartBeat, 0xc);


#endif //PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H
