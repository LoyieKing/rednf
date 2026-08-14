//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_USER_REPEL_BYCHARNAME_H
#define PACKET_MONITOR_USER_REPEL_BYCHARNAME_H

#include "PacketHeader.h"

class Packet_Monitor_User_Repel_ByCharName : public PacketHeader {
public:
    unsigned int m_fieldA;      // +0xa
    unsigned int m_dbid;        // +0xe FindUser
    char m_name[0x1e];          // +0x12
    Packet_Monitor_User_Repel_ByCharName(): PacketHeader(0x4d0,0x30) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_User_Repel_ByCharName, 0x30);


#endif //PACKET_MONITOR_USER_REPEL_BYCHARNAME_H
