//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_ATTENDANCE_INFO_H
#define PACKET_GUILD_ATTENDANCE_INFO_H

#include "PacketHeader.h"

class Packet_Guild_Attendance_Info : public PacketHeader {
public:
    Packet_Guild_Attendance_Info() : PacketHeader(0x1bc6, 0x13){};
};

#endif  // PACKET_GUILD_ATTENDANCE_INFO_H
