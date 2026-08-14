#ifndef PACKET_CHECK_OVERLAPPED_ACCUSATION_H
#define PACKET_CHECK_OVERLAPPED_ACCUSATION_H

#include "PacketHeader.h"

class Packet_Check_Overlapped_Accusation : public PacketHeader {
public:
    unsigned int m_fieldA;      // +0xa
    unsigned int m_fieldE;      // +0xe
    int m_type;                 // +0x12
    char m_pad16[0xc];          // +0x16
    char m_name1[0x1e];         // +0x22
    char m_name2[0x11e];        // +0x40
    char m_result;              // +0x15e
    Packet_Check_Overlapped_Accusation() : PacketHeader(0x1b66, 0x15f){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Check_Overlapped_Accusation, 0x15f);

#endif  // PACKET_CHECK_OVERLAPPED_ACCUSATION_H
