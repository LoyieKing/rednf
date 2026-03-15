//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GAMESERVER2STATISCTICS2DBSERVER_H
#define PACKET_GAMESERVER2STATISCTICS2DBSERVER_H

#include "PacketHeader.h"

class Packet_GameServer2Statisctics2DBServer : public PacketHeader {
public:
    Packet_GameServer2Statisctics2DBServer(): PacketHeader(0x27fd,0x35) {};
};


#endif //PACKET_GAMESERVER2STATISCTICS2DBSERVER_H
