#ifndef PACKET_MONITOR_REPLY_CHARAC_INFO_H_
#define PACKET_MONITOR_REPLY_CHARAC_INFO_H_

#include <string.h>

#include "PacketHeader.h"

class Packet_Monitor_Reply_Charac_Info : public PacketHeader
{
public:
    Packet_Monitor_Reply_Charac_Info() : PacketHeader(0x27df, 0x39)
    {
        *(int*)((char*)this + 0xa) = 0;
        *(int*)((char*)this + 0xe) = -1;
        *(int*)((char*)this + 0x12) = 0;
        *(char*)((char*)this + 0x16) = 0;
        memset((char*)this + 0x17, 0, 0x1e);
        *(unsigned short*)((char*)this + 0x35) = 0;
        *(char*)((char*)this + 0x37) = 0xff;
        *(char*)((char*)this + 0x38) = 0xff;
    }
} __attribute__((packed));

#endif  // PACKET_MONITOR_REPLY_CHARAC_INFO_H_
