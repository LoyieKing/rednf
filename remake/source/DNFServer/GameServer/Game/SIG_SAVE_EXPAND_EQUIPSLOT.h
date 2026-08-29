#ifndef DNF_GAME_SIG_SAVE_EXPAND_EQUIPSLOT_H_
#define DNF_GAME_SIG_SAVE_EXPAND_EQUIPSLOT_H_

// ============================================================================
// SIG_SAVE_EXPAND_EQUIPSLOT —— 扩展装备槽存档
// ORIG ctor W @0x08491c88：+0x4 起 Inven_Item C1 × 0x24（esi=0x23 倒计数，
// 步长 0x3d：0x24×0x3d = 0x894），前 4 字节不触碰；sizeof = 0x898。
// SIG_LOAD_INVENTORY m_expandEquip（+0x14aa4）完整子对象。
// ============================================================================

#include "Inven_Item.h"

#pragma pack(push, 1)
struct SIG_SAVE_EXPAND_EQUIPSLOT
{
    char m_pad0[4];                // +0x000
    Inven_Item m_items[0x24];      // +0x004（0x894）

    SIG_SAVE_EXPAND_EQUIPSLOT() {} // ORIG W 0x08491c88（数组成员隐式构造）
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_EXPAND_EQUIPSLOT) == 0x898,
              "SIG_SAVE_EXPAND_EQUIPSLOT ABI drift");

#endif  // DNF_GAME_SIG_SAVE_EXPAND_EQUIPSLOT_H_
