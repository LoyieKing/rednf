#ifndef PACKET_LIMIT_NPC_BUY_ITEM_UPDATE_H
#define PACKET_LIMIT_NPC_BUY_ITEM_UPDATE_H

// ============================================================================
// ORIG _ZN22LimitNpcBuyItemUpdateC1Ev W @0x084e9570 = PacketHeader(0x27dd,0x1a)
// （W0 批 objdump 实证；Inter_NpcLimitBuyItemInfo 回执包，0x1a 字节 pack(1)）。
// 字段：+0x0a unsigned int / +0x0e unsigned int / +0x12 unsigned int
// （worker 视图命名 m_characNo/m_itemIdx/m_count；m_error 位于 PacketHeader
//   基类可见区——InterDispatcher.cpp 侧 update.m_error 写 +0x04 基类字段）。
// ============================================================================

#include "PacketHeader.h"

class LimitNpcBuyItemUpdate : public PacketHeader {
public:
    unsigned int m_error;      // +0x0a（ORIG 调用点首写 dword）
    unsigned int m_characNo;   // +0x0e
    unsigned int m_itemIdx;    // +0x12
    unsigned int m_count;      // +0x16
    LimitNpcBuyItemUpdate(): PacketHeader(0x27dd, 0x1a) {};
} __attribute__((packed));

TEST_CLASS_SIZE(LimitNpcBuyItemUpdate, 0x1a);

#endif // PACKET_LIMIT_NPC_BUY_ITEM_UPDATE_H
