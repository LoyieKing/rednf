//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_USER_REPEL_BYCHARNAME_H
#define PACKET_MONITOR_USER_REPEL_BYCHARNAME_H

#include "PacketHeader.h"

class Packet_Monitor_User_Repel_ByCharName : public PacketHeader {
public:
    Packet_Monitor_User_Repel_ByCharName(): PacketHeader(0x4d0,0x30) {};
};


#endif //PACKET_MONITOR_USER_REPEL_BYCHARNAME_H
