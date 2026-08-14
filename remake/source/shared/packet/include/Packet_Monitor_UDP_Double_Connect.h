#ifndef PACKET_MONITOR_UDP_DOUBLE_CONNECT_H
#define PACKET_MONITOR_UDP_DOUBLE_CONNECT_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_Double_Connect : public PacketHeader {
public:
    unsigned int m_dbid;        // +0xa
    Packet_Monitor_UDP_Double_Connect() : PacketHeader(0x3ee, 0xe){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_UDP_Double_Connect, 0xe);

#endif  // PACKET_MONITOR_UDP_DOUBLE_CONNECT_H
