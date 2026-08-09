//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_WEB_NOTICE_INGAME_ADVERTISEMENT_H
#define PACKET_WEB_NOTICE_INGAME_ADVERTISEMENT_H

#include "PacketHeader.h"

class Packet_Web_Notice_InGame_Advertisement : public PacketHeader {
public:
    Packet_Web_Notice_InGame_Advertisement() : PacketHeader(0x27e2, 0xa){};
} __attribute__((packed));

#endif  // PACKET_WEB_NOTICE_INGAME_ADVERTISEMENT_H
