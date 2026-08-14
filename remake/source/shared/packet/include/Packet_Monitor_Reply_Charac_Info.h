#ifndef PACKET_MONITOR_REPLY_CHARAC_INFO_H_
#define PACKET_MONITOR_REPLY_CHARAC_INFO_H_

#include <string.h>

#include "PacketHeader.h"

class Packet_Monitor_Reply_Charac_Info : public PacketHeader
{
public:
    unsigned int m_result;           // +0xa
    unsigned int m_idByChannel;      // +0xe
    unsigned int m_targetIdByChannel;// +0x12
    signed char m_channel;           // +0x16
    char m_name[0x1e];               // +0x17
    unsigned short m_level;          // +0x35
    signed char m_job;               // +0x37
    signed char m_growthType;        // +0x38
    Packet_Monitor_Reply_Charac_Info() : PacketHeader(0x27df, 0x39)
    {
        m_result = 0;
        m_idByChannel = (unsigned int)-1;
        m_targetIdByChannel = 0;
        m_channel = 0;
        memset(m_name, 0, 0x1e);
        m_level = 0;
        m_job = (signed char)0xff;
        m_growthType = (signed char)0xff;
    }
} __attribute__((packed));

#endif  // PACKET_MONITOR_REPLY_CHARAC_INFO_H_
