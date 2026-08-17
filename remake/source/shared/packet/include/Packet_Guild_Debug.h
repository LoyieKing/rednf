//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_DEBUG_H
#define PACKET_GUILD_DEBUG_H

#include "PacketHeader.h"

class Packet_Guild_Debug : public PacketHeader {
public:
    unsigned int m_guildKey;  // +0xa
    unsigned int m_charNo;    // +0xe
    int m_mode;               // +0x12（发送端 CGuildDebug::execute：GM 命令对象 this+8；
                              //      guild 端 OnGuildDebug 按 mode 0/1 分发 RefreshAttendanceInfo/RefreshTodayMember）
    int m_field16;            // +0x16（发送端 CGuildDebug::execute：GM 命令对象 this+0xc 透传；
                              //      guild 端 OnGuildDebug（ORIG 0x80892b2）不读此字段。
                              //      深挖结论：命令对象 = game_master::CGuildDebug（CCommand 子类，
                              //      大小 0x14，注册于 CGameMasterMgr ctor 0x82a9804，命令 id 0x42；
                              //      链路 Dispatcher_New_Gmdebug_Command::oldGmRoutine ->
                              //      DoUserDefineCommand -> CParser -> GetCommand(0x42) ->
                              //      SetParser -> executeTemplate(parse→execute)）。
                              //      CGuildDebug::parse（0x84ac720）置 this+8=atoi(args) 并显式清零
                              //      this+0xc/this+0x10；df_game_r 无其它写点，故本字段恒为 0，
                              //      是命令对象未用保留参数（非 gmId/gmCharacNo/gmName），保留占位名）
    int m_field1a;            // +0x1a（发送端 CGuildDebug::execute：GM 命令对象 this+0x10 透传；
                              //      guild 端 OnGuildDebug 不读此字段；同 +0x16：parse 显式清零
                              //      this+0x10 且无其它写入点，恒为 0，保留占位名）

    Packet_Guild_Debug() : PacketHeader(0x1bc9, 0x1e),
        m_guildKey(0), m_charNo(0), m_mode(0), m_field16(0), m_field1a(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Guild_Debug, 0x1e);

#endif  // PACKET_GUILD_DEBUG_H
