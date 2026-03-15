//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REQUEST_CHARAC_TOWER_UPDATE_RANKING_H
#define PACKET_REQUEST_CHARAC_TOWER_UPDATE_RANKING_H

#include "PacketHeader.h"

class Packet_Request_Charac_Tower_Update_Ranking : public PacketHeader {
public:
    Packet_Request_Charac_Tower_Update_Ranking(): PacketHeader(0x4ce,0x1a) {};
};


#endif //PACKET_REQUEST_CHARAC_TOWER_UPDATE_RANKING_H
