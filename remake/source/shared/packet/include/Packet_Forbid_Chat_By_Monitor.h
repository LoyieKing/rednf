//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FORBID_CHAT_BY_MONITOR_H
#define PACKET_FORBID_CHAT_BY_MONITOR_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Forbid_Chat_By_Monitor : public PacketHeader {
public:
    unsigned int m_secs;            // +0xa
    char m_padE[4];                 // +0xe（第三轮穷举 2026-08-16：df_game_r
                                    //      Dispatcher_GM_Command::dispatch_sig（语料两副本
                                    //      @81fa19e/@29583，case 5 forbidchat）构造后仅写
                                    //      +0xa(m_secs)/+0x12(m_name) 并以 0x30 整包发 monitor，
                                    //      +0xe 无任何写入；monitor OnForbidChat 只读
                                    //      m_secs/m_name 整包转发到全部游戏服；无消费端证据，
                                    //      保留占位名）
    char m_name[0x1e];              // +0x12
    Packet_Forbid_Chat_By_Monitor() : PacketHeader(0x9dd, 0x30)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Forbid_Chat_By_Monitor, 0x30);

#endif  // PACKET_FORBID_CHAT_BY_MONITOR_H
