//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_REQUEST_MEMBER_ENTER_H
#define PACKET_MONITOR_REQUEST_MEMBER_ENTER_H

#include "PacketHeader.h"

class Packet_Monitor_Request_Member_Enter : public PacketHeader {
public:
    Packet_Monitor_Request_Member_Enter(): PacketHeader(0x4b7,0x30) {};
};


#endif //PACKET_MONITOR_REQUEST_MEMBER_ENTER_H
