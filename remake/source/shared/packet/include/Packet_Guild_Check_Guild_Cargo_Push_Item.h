//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H
#define PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H

#include "PacketHeader.h"

class Packet_Guild_Check_Guild_Cargo_Push_Item : public PacketHeader {
public:
    unsigned int m_guildKey;     // +0xa（Dispatcher_GuildCargoPushItem::process 81d10d4:
                                 //          local_4c = get_charac_guildkey）
    unsigned int m_charNo;       // +0xe（process: getCurCharacNo）
    int m_slot;                  // +0x12 公会仓库目标槽位（process: 客户端请求消息 +0x18 的 ushort
                                 //          零扩展；guild 端 OnGuildCargoCheckPushItem:
                                 //          CheckInsertItem(itemId,count,slot=m_slot,...)，echo 到 reply.m13）
    int m_itemId;                // +0x16（process: 客户端消息 +0x10 的 item code；guild 端
                                 //          CheckInsertItem 第一参 itemId）
    int m_count;                 // +0x1a（process: 客户端消息 +0x14；CheckInsertItem count）
    int m_maxStack;              // +0x1e（process: is_stackable ? getStackableLimit : 1；
                                 //          CheckInsertItem maxStack）
    unsigned char m_stackable;   // +0x22（process: CItem::is_stackable；CheckInsertItem stackable）
    unsigned char m_invenType;   // +0x23（process: 客户端消息 +0xd（GetInvenTypeFromItemSpace 的
                                 //          库存类型）；guild 端 echo 到 reply.m24）
    unsigned short m_invenSlot;  // +0x24（process: 客户端消息 +0xe 物品所在库存槽位；
                                 //          echo 到 reply.m25）
    Packet_Guild_Check_Guild_Cargo_Push_Item() : PacketHeader(0x70c, 0x26),
        m_guildKey(0), m_charNo(0), m_slot(0), m_itemId(0), m_count(0), m_maxStack(0), m_stackable(0), m_invenType(0), m_invenSlot(0)
    {

    };
} __attribute__((packed));


#endif //PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H
