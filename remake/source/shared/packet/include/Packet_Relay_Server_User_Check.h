#ifndef PACKET_RELAY_SERVER_USER_CHECK_H
#define PACKET_RELAY_SERVER_USER_CHECK_H

#include "PacketHeader.h"

class Packet_Relay_Server_User_Check : public PacketHeader {
public:
    unsigned int m_dbid;        // +0xa
    unsigned char m_channel;    // +0xe 入站频道，回写在线标记
    Packet_Relay_Server_User_Check() : PacketHeader(0x9c4, 0xf){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Relay_Server_User_Check, 0xf);

#endif  // PACKET_RELAY_SERVER_USER_CHECK_H
