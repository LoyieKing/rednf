//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GAME_SERVER_REGIST_H
#define PACKET_GAME_SERVER_REGIST_H

#include "PacketHeader.h"

class Packet_Game_Server_Regist : public PacketHeader {
public:
    Packet_Game_Server_Regist() : PacketHeader(0x1f41, 0x1f){};
};

#endif  // PACKET_GAME_SERVER_REGIST_H
