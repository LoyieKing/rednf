//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GAMESERVER2STATISCTICS2DBSERVER_H
#define PACKET_GAMESERVER2STATISCTICS2DBSERVER_H

#include "PacketHeader.h"
#include <string.h>

class Packet_GameServer2Statisctics2DBServer : public PacketHeader {
public:
    short m_serverGroup;  // +a（dispatch_sig: CEnvironment::get_server_group；DB p2p_connect_success_rate.server_group）
    unsigned char m_connectedType;  // +c（dispatch_sig: 源结构 +0xd 字节；Statics 接收端 OnHolePunchingSuccessRateStatistic 直拷 pck->m_connectedType；DB connected_type）
    int m_requiredTime;  // +d（dispatch_sig: 源结构 +0xe 字；Statics 接收端 pck->m_requiredTime；DB required_time）
    int m_checkTime;  // +11（dispatch_sig: 源结构 +0x12 字；Statics 接收端 pck->m_checkTime；DB check_time）
    char m_nationCode[0x10];  // +15..+24（dispatch_sig: RestrictGeolocation::getCountryCode 写入；Statics 接收端 strncpy(0x10)；DB nation_code）
    char m_peerAddress[0x10];  // +25..+34（dispatch_sig: CNetwork::GetPeerIP2 写入；Statics 接收端 strncpy(0x10)；DB peer_address）

    Packet_GameServer2Statisctics2DBServer() : PacketHeader(0x27fd, 0x35),
        m_serverGroup(0), m_connectedType(255), m_requiredTime(0), m_checkTime(0)
    {
    memset(m_nationCode, 0, sizeof(m_nationCode));
    memset(m_peerAddress, 0, sizeof(m_peerAddress));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_GameServer2Statisctics2DBServer, 0x35);


#endif //PACKET_GAMESERVER2STATISCTICS2DBSERVER_H
