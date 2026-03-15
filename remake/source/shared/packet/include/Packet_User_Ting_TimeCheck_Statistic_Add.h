//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_USER_TING_TIMECHECK_STATISTIC_ADD_H
#define PACKET_USER_TING_TIMECHECK_STATISTIC_ADD_H

#include "PacketHeader.h"

class Packet_User_Ting_TimeCheck_Statistic_Add : public PacketHeader {
public:
    Packet_User_Ting_TimeCheck_Statistic_Add(): PacketHeader(0xc45,0x12) {};
};


#endif //PACKET_USER_TING_TIMECHECK_STATISTIC_ADD_H
