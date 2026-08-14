//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H
#define PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H

#include "PacketHeader.h"

class Packet_Game_Monitor_GM_Village_Attacked : public PacketHeader {
public:
    unsigned char m_flag;   // +0xa  0=SetRealConfig
    unsigned int m_a;       // +0xb
    unsigned int m_b;       // +0xf
    unsigned int m_c;       // +0x13
    Packet_Game_Monitor_GM_Village_Attacked(): PacketHeader(0x234a,0x17) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Game_Monitor_GM_Village_Attacked, 0x17);

#endif //PACKET_GAME_MONITOR_GM_VILLAGE_ATTACKED_H
