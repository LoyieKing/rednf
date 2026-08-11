#ifndef PACKET_DBMW_CHANGE_CHAR_NAME_H_
#define PACKET_DBMW_CHANGE_CHAR_NAME_H_

#include <string.h>

#include "PacketHeader.h"

class Packet_DBMW_Change_Char_Name : public PacketHeader
{
public:
    char m_type;              // +0xa
    unsigned int m_charNo;    // +0xb
    char m_name[0x1e];        // +0xf
    Packet_DBMW_Change_Char_Name() : PacketHeader(0x451, 0x2d)
    {
        m_type = 0;
        m_charNo = 0;
        memset(m_name, 0, 0x1e);
    }
} __attribute__((packed));

#endif  // PACKET_DBMW_CHANGE_CHAR_NAME_H_
