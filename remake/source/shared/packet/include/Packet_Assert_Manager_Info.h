//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_ASSERT_MANAGER_INFO_H
#define PACKET_ASSERT_MANAGER_INFO_H

#include "PacketHeader.h"

class Packet_Assert_Manager_Info : public PacketHeader {
public:
    Packet_Assert_Manager_Info() : PacketHeader(0xc41, 0x214){};
};

#endif  // PACKET_ASSERT_MANAGER_INFO_H
