//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_CHAT_HYPER_LINK_H
#define PACKET_MONITOR_MEMBER_CHAT_HYPER_LINK_H

#include "PacketHeader.h"

class Packet_Monitor_Member_Chat_Hyper_Link : public PacketHeader {
public:
    Packet_Monitor_Member_Chat_Hyper_Link(): PacketHeader(0x271a,0x248) {};
};


#endif //PACKET_MONITOR_MEMBER_CHAT_HYPER_LINK_H
