//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CHANGE_CHAR_NAME_H
#define PACKET_CHANGE_CHAR_NAME_H

#include "PacketHeader.h"

class Packet_Change_Char_Name : public PacketHeader {
public:
    Packet_Change_Char_Name() : PacketHeader(0x451, 0x34){};
};

#endif  // PACKET_CHANGE_CHAR_NAME_H
