//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_FIND_FACTORY_HUB_USER_H
#define PACKET_MONITOR_FIND_FACTORY_HUB_USER_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Find_Factory_Hub_User : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa 请求者
    unsigned char m_nameLen;    // +0xe
    char m_name[0x1e];          // +0xf
    unsigned short m_field2e;   // +0x2d
    unsigned int m_field30;     // +0x2f
    Packet_Monitor_Find_Factory_Hub_User() : PacketHeader(0x100e, 0x33),
        m_charNo(0), m_nameLen(0), m_field2e(0), m_field30(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Find_Factory_Hub_User, 0x33);


#endif //PACKET_MONITOR_FIND_FACTORY_HUB_USER_H
