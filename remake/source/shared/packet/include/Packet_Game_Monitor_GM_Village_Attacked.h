//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H
#define PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H

#include "PacketHeader.h"

class Packet_Game_Monitor_GM_Village_Attacked : public PacketHeader {
public:
    Packet_Game_Monitor_GM_Village_Attacked(): PacketHeader(0x234a,0x17) {};
};


#endif //PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H
