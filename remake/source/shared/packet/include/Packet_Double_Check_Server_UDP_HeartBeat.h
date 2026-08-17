//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H
#define PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H

#include "PacketHeader.h"

class Packet_Double_Check_Server_UDP_HeartBeat : public PacketHeader {
public:
    unsigned char m_channel;  // +a（SendHeartBeat param_1；接收端 coserver OnHeartBeat 实测：
                              //     pkt[0xa] 即 channel，校验 1..0xbe，ResetHeartBeat(group,channel)）
    unsigned char m_group;    // +b（SendHeartBeat param_2；接收端 coserver OnHeartBeat 实测：
                              //     pkt[0xb] 即 group，校验 group<=100，FindGameServer(group,channel)）

    Packet_Double_Check_Server_UDP_HeartBeat() : PacketHeader(0xbba, 0xc),
        m_channel(255), m_group(255)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Double_Check_Server_UDP_HeartBeat, 0xc);


#endif //PACKET_DOUBLE_CHECK_SERVER_UDP_HEARTBEAT_H
