//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CUTOFF_UDP_REPLY_USERINFO_H
#define PACKET_CUTOFF_UDP_REPLY_USERINFO_H

#include "PacketHeader.h"

class Packet_CutOff_UDP_Reply_UserInfo : public PacketHeader {
public:
    Packet_CutOff_UDP_Reply_UserInfo(): PacketHeader(0x3f9,0xafe) {};
};


#endif //PACKET_CUTOFF_UDP_REPLY_USERINFO_H
