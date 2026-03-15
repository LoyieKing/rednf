//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_ENTER_REPLY_H
#define PACKET_MONITOR_MEMBER_ENTER_REPLY_H

#include "PacketHeader.h"

class Packet_Monitor_Member_Enter_Reply : public PacketHeader {
public:
    Packet_Monitor_Member_Enter_Reply(): PacketHeader(0x4b9,0x13) {};
};


#endif //PACKET_MONITOR_MEMBER_ENTER_REPLY_H
