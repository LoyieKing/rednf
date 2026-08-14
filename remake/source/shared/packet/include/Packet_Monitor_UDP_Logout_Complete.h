//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_LOGOUT_COMPLETE_H
#define PACKET_MONITOR_UDP_LOGOUT_COMPLETE_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_Logout_Complete : public PacketHeader {
public:
    unsigned int m_dbid;            // +0xa
    unsigned char m_channel;        // +0xe DeleteProhibitUser 频道
    Packet_Monitor_UDP_Logout_Complete(): PacketHeader(0x3f4,0xf) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_UDP_Logout_Complete, 0xf);


#endif //PACKET_MONITOR_UDP_LOGOUT_COMPLETE_H
