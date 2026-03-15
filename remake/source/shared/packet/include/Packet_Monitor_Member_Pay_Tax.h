//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEMBER_PAY_TAX_H
#define PACKET_MONITOR_MEMBER_PAY_TAX_H

#include "PacketHeader.h"

class Packet_Monitor_Member_Pay_Tax : public PacketHeader {
public:
    Packet_Monitor_Member_Pay_Tax() : PacketHeader(0x4c0, 0x16){};
};

#endif  // PACKET_MONITOR_MEMBER_PAY_TAX_H
