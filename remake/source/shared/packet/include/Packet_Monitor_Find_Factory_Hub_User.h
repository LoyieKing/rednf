//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_FIND_FACTORY_HUB_USER_H
#define PACKET_MONITOR_FIND_FACTORY_HUB_USER_H

#include "PacketHeader.h"

class Packet_Monitor_Find_Factory_Hub_User : public PacketHeader {
public:
    Packet_Monitor_Find_Factory_Hub_User(): PacketHeader(0x100e,0x33) {};
};


#endif //PACKET_MONITOR_FIND_FACTORY_HUB_USER_H
