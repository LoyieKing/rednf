//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_ATTENDANCE_INFO_H
#define PACKET_GUILD_ATTENDANCE_INFO_H

#include "PacketHeader.h"

class Packet_Guild_Attendance_Info : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_characNo;    // +0xe（process：CUserCharacInfo::getCurCharacNo）
    unsigned char m_field12;    // +0x12（发送端 Dispatcher_GuildAttendanceInfo::process 原样拷贝
                                //      MSG_BASE[0xd]（客户端请求参数）。回传链路穷举：客户端
                                //      MSG_GUILD_ATTENDANCE 请求体仅 1 字节，
                                //      Dispatcher_GuildAttendanceInfo::read（0x81d543c）读入
                                //      MSG_BASE+0xd -> process（0x81d5492）拷贝到本字段 ->
                                //      guild OnGuildAttendanceInfo（ORIG 0x808902a）回传到
                                //      Packet_Guild_Attendance_Info_Reply +0x12（m_c）随
                                //      STAttendanceInfo 送回 -> game 端
                                //      Inter_GuildAttendanceInfo::dispatch_sig（0x84e4052）
                                //      原样转发回客户端；game/guild 两端均无分支消费该字节
                                //      （纯回显令牌），参数语义无证据，保留占位名）
    Packet_Guild_Attendance_Info() : PacketHeader(0x1bc6, 0x13),
        m_guildKey(0), m_characNo(0), m_field12(0)
    {

    };
} __attribute__((packed));

#endif  // PACKET_GUILD_ATTENDANCE_INFO_H
