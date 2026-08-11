#ifndef PACKET_MONITOR_REPLY_CHARAC_INFO_H_
#define PACKET_MONITOR_REPLY_CHARAC_INFO_H_

#include <string.h>

#include "PacketHeader.h"

class Packet_Monitor_Reply_Charac_Info : public PacketHeader
{
public:
    unsigned int m_fieldA;      // +0xa
    unsigned int m_fieldE;      // +0xe
    unsigned int m_field12;     // +0x12
    char m_field16;             // +0x16
    char m_name[0x1e];          // +0x17
    unsigned short m_field35;   // +0x35
    char m_field37;             // +0x37
    char m_field38;             // +0x38
    Packet_Monitor_Reply_Charac_Info() : PacketHeader(0x27df, 0x39)
    {
        m_fieldA = 0;
        m_fieldE = -1;
        m_field12 = 0;
        m_field16 = 0;
        memset(m_name, 0, 0x1e);
        m_field35 = 0;
        m_field37 = 0xff;
        m_field38 = 0xff;
    }
} __attribute__((packed));

#endif  // PACKET_MONITOR_REPLY_CHARAC_INFO_H_
