//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_NOTIFY_NEW_MAIL_H
#define PACKET_MONITOR_NOTIFY_NEW_MAIL_H

#include "PacketHeader.h"

class Packet_Monitor_Notify_New_Mail : public PacketHeader {
public:
    Packet_Monitor_Notify_New_Mail() : PacketHeader(0x514, 0x12){};
};

#endif  // PACKET_MONITOR_NOTIFY_NEW_MAIL_H
