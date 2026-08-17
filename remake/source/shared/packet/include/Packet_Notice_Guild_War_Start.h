//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_START_H
#define PACKET_NOTICE_GUILD_WAR_START_H

#include "PacketHeader.h"

class Packet_Notice_Guild_War_Start : public PacketHeader {
public:
    unsigned char m_channel;  // +0xa（2026-08-16 深挖定名：与 Packet_Notice_Guild_War_End 同构；
                              //      game 侧 SendGuildWarStart@846df90 写本字节（param_1），
                              //      CMonitorServerProxy::SendGuildWarStart@84710e6 同构写；
                              //      guild ORIG OnNoticeGuildWarStart 反汇编 movzbl 0xa 读取）
    Packet_Notice_Guild_War_Start() : PacketHeader(0x416, 0xb){};
} __attribute__((packed));

#endif  // PACKET_NOTICE_GUILD_WAR_START_H
