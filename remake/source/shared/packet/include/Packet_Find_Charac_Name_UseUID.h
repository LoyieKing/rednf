#ifndef PACKET_FIND_CHARAC_NAME_USEUID_H
#define PACKET_FIND_CHARAC_NAME_USEUID_H

#include "PacketHeader.h"

class Packet_Find_Charac_Name_UseUID : public PacketHeader {
public:
    unsigned int m_dbid;        // +0xa
    unsigned int m_charNo;      // +0xe
    unsigned int m_len;         // +0x12
    char m_name[0x1e];          // +0x16
    Packet_Find_Charac_Name_UseUID() : PacketHeader(0x1f45, 0x34){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Find_Charac_Name_UseUID, 0x34);

#endif  // PACKET_FIND_CHARAC_NAME_USEUID_H
