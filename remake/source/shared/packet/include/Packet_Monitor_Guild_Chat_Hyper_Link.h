//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H
#define PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H

#include "PacketHeader.h"
#include <string.h>
// [W10-GuildB · 机理 X] m_items 必须为「真类型数组」⇒ ARRAY_REF decay 生成
//   `lea 0x13(%reg),%dst`（与 ORIG 一致）；旧形 `char m_items[0x138]` + 调用点 cast
//   生成 `add $0x13; mov`（探针 lea2.cpp 实证：真数组 `Item items[3]` ⇒ lea，
//   `char[] + cast` ⇒ add+mov）。
//   hyperlink_item_info 权威定义（0x68，POD）在 Member_Chat_Hyper_Link 头；
//   Guild/DNFGuild.h 的 0x40 遗留重复定义已随之删除（ODR 清理，Main 2026-09-15 裁决 A）。
#include "Packet_Monitor_Member_Chat_Hyper_Link.h"

class Packet_Monitor_Guild_Chat_Hyper_Link : public PacketHeader {
public:
    // [P-GPT] ORIG 实证（guild OnNoticeGuildChatMsgHyperLink：FindUser_CharNo 实参=+0xe、
    //   FindGuild 实参=+0xa；game send_chat_msg_hyper_link.inc 注释亦记 +0xa 先写 guildkey）
    //   ⇒ charNo@+0xe、guildKey@+0xa。宽度/偏移/sizeof 不变，纯校正名字与偏移的对应关系。
    unsigned int m_guildKey;  // +0xa
    unsigned int m_charNo;    // +0xe
    unsigned char m_itemCount; // +0x12（send_chat_msg_hyper_link case 6：MSG+0x13b 超链接道具个数；
                               //          Guild OnNoticeGuildChatMsgHyperLink 作 itemCount 参数）
    hyperlink_item_info m_items[3]; // +0x13（3×0x68 = 0x138，sizeof/偏移不变）
    unsigned char m_msgLen;   // +0x14b
    char m_msg[0x100];        // +0x14c
    Packet_Monitor_Guild_Chat_Hyper_Link() : PacketHeader(0x2718, 0x24c),
        m_charNo(0), m_guildKey(0), m_itemCount(0), m_msgLen(0)
    {
    // ORIG 0x086d1afb memset(this+0x14c,0x100) 先，0x086d1b19 memset(this+0x13,0x138) 后
    memset(m_msg, 0, sizeof(m_msg));
    memset(m_items, 0, sizeof(m_items));
    };
} __attribute__((packed));


#endif //PACKET_MONITOR_GUILD_CHAT_HYPER_LINK_H
