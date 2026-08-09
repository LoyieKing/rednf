//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MANAGER_CONNECT_OK_H
#define PACKET_MONITOR_MANAGER_CONNECT_OK_H

#include "PacketHeader.h"

class Packet_Monitor_Manager_Connect_OK : public PacketHeader {
public:
    Packet_Monitor_Manager_Connect_OK() : PacketHeader(0x578, 0xa){};
} __attribute__((packed));

#endif  // PACKET_MONITOR_MANAGER_CONNECT_OK_H
