#ifndef PACKET_LIMIT_NPC_BUY_ITEM_UPDATE_H
#define PACKET_LIMIT_NPC_BUY_ITEM_UPDATE_H

// ============================================================================
// ORIG _ZN22LimitNpcBuyItemUpdateC1Ev W @0x084e9570 = PacketHeader(0x27dd,0x1a)
// （W0 批 objdump 实证；Inter_NpcLimitBuyItemInfo 回执包，0x1a 字节 pack(1)）。
// [Lane30InterDispatcherPktView · §8.3 字段化] 偏移/命名按 ORIG
//   Inter_NpcLimitBuyItemInfo::dispatch_sig @0x084e65a2 逐写序改正
//   （base PacketHeader 0xa + 四个 dword，命名与 Monitor/LimitNpcBuyItem.h
//    的同名类一致）：
//     +0x0a m_itemId      —— 084e6604 `mov 0x16(%eax); mov %eax,-0x24(%ebp)`（msg.m_itemId）、
//                            成功路 084e6a52 `mov 0x4(%eax); mov %eax,-0x24(%ebp)`（this->m_error=item id）
//     +0x0e m_charNo      —— 084e6601 `msg+0x12 → -0x20` / 084e6a6b get_charac_no(-1)
//     +0x12 m_cancelCount—— 084e660a `msg+0x1a → -0x1c`
//     +0x16 m_errorNo     —— 084e65f1 `movl $0x3,-0x18`（错误码 3）/ 084e6a48 `movl $0x0,-0x18`（成功）
//   （旧版命名 m_error/m_characNo/m_itemIdx/m_count 与写点错位一格，已由本次修正；
//     消费点仅 InterDispatcher.cpp。）
// ============================================================================

#include "PacketHeader.h"

class LimitNpcBuyItemUpdate : public PacketHeader {
public:
    unsigned int m_itemId;        // +0x0a
    unsigned int m_charNo;        // +0x0e
    unsigned int m_cancelCount;   // +0x12
    unsigned int m_errorNo;       // +0x16
    // ORIG C1 0x084e9570：PacketHeader 后连清 +0xa/+0xe/+0x12/+0x16 四 dword
    LimitNpcBuyItemUpdate(): PacketHeader(0x27dd, 0x1a),
        m_itemId(0), m_charNo(0), m_cancelCount(0), m_errorNo(0) {};
} __attribute__((packed));

TEST_CLASS_SIZE(LimitNpcBuyItemUpdate, 0x1a);

#endif // PACKET_LIMIT_NPC_BUY_ITEM_UPDATE_H
