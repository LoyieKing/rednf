//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_SYNC_GM_LIST_H
#define PACKET_SYNC_GM_LIST_H

#include "PacketHeader.h"

class Packet_Sync_GM_List : public PacketHeader {
public:
    Packet_Sync_GM_List() : PacketHeader(0x1f4d, 0x70){};
};

#endif  // PACKET_SYNC_GM_LIST_H
