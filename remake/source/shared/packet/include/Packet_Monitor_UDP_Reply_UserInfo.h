//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_REPLY_USERINFO_H
#define PACKET_MONITOR_UDP_REPLY_USERINFO_H

#include "PacketHeader.h"

class Packet_Monitor_UDP_Reply_UserInfo : public PacketHeader {
public:
    Packet_Monitor_UDP_Reply_UserInfo(): PacketHeader(0x3ea,0x16e6) {};
};


#endif //PACKET_MONITOR_UDP_REPLY_USERINFO_H
