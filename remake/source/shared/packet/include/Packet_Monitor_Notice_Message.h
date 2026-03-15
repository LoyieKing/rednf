//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_NOTICE_MESSAGE_H
#define PACKET_MONITOR_NOTICE_MESSAGE_H

#include "PacketHeader.h"

class Packet_Monitor_Notice_Message : public PacketHeader {
public:
    Packet_Monitor_Notice_Message() : PacketHeader(0x9dc, 0x10b){};
};

#endif  // PACKET_MONITOR_NOTICE_MESSAGE_H
