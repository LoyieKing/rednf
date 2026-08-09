#ifndef PACKET_DBMW_CHANGE_CHAR_NAME_H_
#define PACKET_DBMW_CHANGE_CHAR_NAME_H_

#include <string.h>

#include "PacketHeader.h"

class Packet_DBMW_Change_Char_Name : public PacketHeader
{
public:
    Packet_DBMW_Change_Char_Name() : PacketHeader(0x451, 0x2d)
    {
        *(char*)((char*)this + 0xa) = 0;
        *(int*)((char*)this + 0xb) = 0;
        memset((char*)this + 0xf, 0, 0x1e);
    }
} __attribute__((packed));

#endif  // PACKET_DBMW_CHANGE_CHAR_NAME_H_
