// Packet_Guild_Check_Guild_Cargo_Push_Item：完整布局自 shared/packet/include 权威版
// （guild 端 DNFPacketTranslater::OnGuildCargoCheckPushItem 消费字段 + gameserver
// Dispatcher_GuildCargoPushItem::process ORIG 0x081d10d4 写点实证）。
// ORIG 仅导出弱内联 ctor W @0x0822bec2；sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H_
#define DNF_GAME_PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H_

#include "PacketHeader.h"

class Packet_Guild_Check_Guild_Cargo_Push_Item : public PacketHeader {
public:
    unsigned int m_guildKey;     // +0xa（process: get_charac_guildkey）
    unsigned int m_charNo;       // +0xe（process: getCurCharacNo）
    int m_slot;                  // +0x12 公会仓库目标槽位（process: 客户端消息 +0x18 ushort）
    int m_itemId;                // +0x16（process: 客户端消息 +0x10 item code）
    int m_count;                 // +0x1a（process: 客户端消息 +0x14 count）
    int m_maxStack;              // +0x1e（process: is_stackable ? getStackableLimit : 1）
    unsigned char m_stackable;   // +0x22（process: CItem::is_stackable）
    unsigned char m_invenType;   // +0x23（process: 客户端消息 +0xd 库存类型）
    unsigned short m_invenSlot;  // +0x24（process: 客户端消息 +0xe 物品库存槽位）
    Packet_Guild_Check_Guild_Cargo_Push_Item() : PacketHeader(0x70c, 0x26),
        m_guildKey(0), m_charNo(0), m_slot(0), m_itemId(0), m_count(0), m_maxStack(0),
        m_stackable(0), m_invenType(0), m_invenSlot(0)
    {
    }
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_GUILD_CHECK_GUILD_CARGO_PUSH_ITEM_H_
