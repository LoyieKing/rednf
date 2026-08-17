//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_P2P_STATISTICS_H
#define PACKET_P2P_STATISTICS_H

#include "PacketHeader.h"

class Packet_P2P_Statistics : public PacketHeader {
public:
    int m_p2pUser;               // 0xa  p2p_statistics.p2p_user（Dispatcher_P2P_Statistics "p2p statistics num"；statics AddP2PStatistic m_p2pUser）
    int m_relayUser;             // 0xe  p2p_statistics.relay_user（"relay statistics num"；AddP2PStatistic m_relayUser）
    unsigned char m_serverGroup; // 0x12 CUser::GetServerGroup（AddP2PStatistic m_serverGroup）
    unsigned short m_p2pMinPing; // 0x13 AddP2PStatistic minPing(+0x13)
    unsigned short m_p2pMaxPing; // 0x15 AddP2PStatistic maxPing(+0x15)
    unsigned short m_p2pAvgPing; // 0x17 AddP2PStatistic sumPing avg(+0x17)
    int m_p2pOverPing100;        // 0x19 AddP2PStatistic m_p2pOverPing100
    int m_p2pOverPing200;        // 0x1d AddP2PStatistic m_p2pOverPing200
    int m_p2pOverPing300;        // 0x21 AddP2PStatistic m_p2pOverPing300
    int m_p2pOverPing400;        // 0x25 AddP2PStatistic m_p2pOverPing400
    unsigned short m_relayMinPing; // 0x29 AddP2PStatistic minPing(+0x29)
    unsigned short m_relayMaxPing; // 0x2b AddP2PStatistic maxPing(+0x2b)
    unsigned short m_relayAvgPing; // 0x2d AddP2PStatistic sumPing avg(+0x2d)
    int m_relayOverPing100;      // 0x2f AddP2PStatistic m_relayOverPing100
    int m_relayOverPing200;      // 0x33 AddP2PStatistic m_relayOverPing200
    int m_relayOverPing300;      // 0x37 AddP2PStatistic m_relayOverPing300
    int m_relayOverPing400;      // 0x3b AddP2PStatistic m_relayOverPing400
    Packet_P2P_Statistics() : PacketHeader(0x27e1, 0x3f)
    {
        m_p2pUser = 0;
        m_relayUser = 0;
        m_serverGroup = 0;
        m_p2pMinPing = 0;
        m_p2pMaxPing = 0;
        m_p2pAvgPing = 0;
        m_p2pOverPing100 = 0;
        m_p2pOverPing200 = 0;
        m_p2pOverPing300 = 0;
        m_p2pOverPing400 = 0;
        m_relayMinPing = 0;
        m_relayMaxPing = 0;
        m_relayAvgPing = 0;
        m_relayOverPing100 = 0;
        m_relayOverPing200 = 0;
        m_relayOverPing300 = 0;
        m_relayOverPing400 = 0;
    }
} __attribute__((packed));

#endif  // PACKET_P2P_STATISTICS_H
