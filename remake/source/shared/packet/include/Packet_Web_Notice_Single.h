//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_WEB_NOTICE_SINGLE_H
#define PACKET_WEB_NOTICE_SINGLE_H

#include "PacketHeader.h"

class Packet_Web_Notice_Single : public PacketHeader {
public:
    Packet_Web_Notice_Single() : PacketHeader(0x9e0, 0x10a){};
} __attribute__((packed));

#endif  // PACKET_WEB_NOTICE_SINGLE_H
