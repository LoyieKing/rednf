//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FILE_STATISTIC_H
#define PACKET_FILE_STATISTIC_H

#include "PacketHeader.h"

class Packet_File_Statistic : public PacketHeader {
public:
    Packet_File_Statistic() : PacketHeader(0x27f3, 0x90a){};
};

#endif  // PACKET_FILE_STATISTIC_H
