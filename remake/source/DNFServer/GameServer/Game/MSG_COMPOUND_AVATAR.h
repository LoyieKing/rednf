// df_game_r Game/ MSG_COMPOUND_AVATAR 权威布局（唯一声明点；PacketTypes.h 拆分冻结期间
// 以独立头承载，解冻后由 Main 归口——同 MSG_DIE_ASSAULT_PLAYER.h 先例）。
//
// 类型证据：
//   1) ORIG nm -S --defined-only 仅有按 `19MSG_COMPOUND_AVATAR` 作参数类型的函数符号
//      （0x083338c2 ProcCompound / 0x08331efe _MakeCompoundInfo），无该类型 C1/C2/D1/D2/
//      typeinfo → 纯 POD，不派生 MSG_BASE（同 PacketTypes.h 内 standalone POD 口径）。
//   2) 尺寸/偏移：Dispatcher_Compound_Avatar::dispatch_sig 0x081ead40（报告
//      docs/class_func_reports/Dispatcher_Compound_Avatar/dispatch_sig.md）栈区
//      -0x2f..-0xc（0x23 字节）按序读入：
//        get_short(short&)      → +0x0d
//        get_short(short&)      → +0x0f
//        get_int(unsigned long&)-> +0x11
//        get_short(short&)      → +0x15
//        get_int(unsigned long&)-> +0x17
//        get_int(unsigned long&)-> +0x1b
//        get_short(short&)      → +0x1f
//        get_byte(unsigned char&)-> +0x21
//        get_byte(char&)        → +0x22
//      末项地址 = 基址 +0x22 + 1 字节 → 数据区 0x23；包头 0x00..+0x0c（首字段落 +0x0d，
//      与 PacketTypes.h 内 MSG_BASE 0xd 前缀一致）。
//   3) 消费侧宽度证据（_MakeCompoundInfo 0x08331efe）：+0x0d/+0x0f/+0x15 `movzwl`、
//      +0x11/+0x17/+0x1b `mov`(4B)、+0x1f `movzwl`、+0x21/+0x22 `movzbl`。
//
// 字段名为数据流命名（读点/写点见各行注释）；无 getter/setter 可考的语义标【推断】。
#ifndef DNF_MSG_COMPOUND_AVATAR_H
#define DNF_MSG_COMPOUND_AVATAR_H

#include <cstddef>

#pragma pack(push, 1)
struct MSG_COMPOUND_AVATAR
{
    char m_head[0xd];                // +0x00..+0x0c 包头（ORIG 本函数不读）
    unsigned short m_avatarSlot;     // +0x0d（movzwl → tagCompoundInfo+0x90；
                                     //   CInventory::GetInvenSlot(1,(short)) 槽位）
    unsigned short m_materialSlotA;  // +0x0f（movzwl → info+0x08 + GetInvenSlot(2,(short))
                                     //   材料①槽位；dispatch_sig 另以 CheckItemLock(2,(short)) 校验）
    unsigned int m_materialItemIdA;  // +0x11（mov 4B → CDataManager::find_item 入参，
                                     //   结果落 info+0x88 脚本①；dispatch_sig get_int(unsigned long&)）
    unsigned short m_materialSlotB;  // +0x15（movzwl → info+0x48 + GetInvenSlot(2,(short))
                                     //   材料②槽位；dispatch_sig CheckItemLock 第二参）
    unsigned int m_materialItemIdB;  // +0x17（mov 4B → find_item，结果落 info+0x8c 脚本②）
    int m_optionIdx;                 // +0x1b（mov 4B → info+0xd0；_CheckCondition 与选项向量
                                     //   first 比对、_ProcCompoundCore 直接取作结果下标）【推断】名
    unsigned short m_slot;           // +0x1f（movzwl → info+0xd4；_CheckCondition 判 >99、
                                     //   _ProcCompoundCore 以随机槽位覆盖）
    unsigned char m_optionCheckFlag; // +0x21（movzbl 与 1 比对 → info+0xd6；==1 跳过选项池
                                     //   校验分支）【推断】名
    unsigned char m_type;            // +0x22（movzbl → info+0xe8 合成类型，0/1/3/4 分派
                                     //   m_tailA/m_tailB/m_tailC/m_cube.m_vec）
};                                   // sizeof == 0x23（ORIG 栈区 -0x2f..-0xc 实证）
#pragma pack(pop)

static_assert(sizeof(MSG_COMPOUND_AVATAR) == 0x23,
              "MSG_COMPOUND_AVATAR ABI drift (ORIG size 0x23)");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_avatarSlot) == 0x0d,
              "MSG_COMPOUND_AVATAR m_avatarSlot offset drift");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_materialSlotA) == 0x0f,
              "MSG_COMPOUND_AVATAR m_materialSlotA offset drift");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_materialItemIdA) == 0x11,
              "MSG_COMPOUND_AVATAR m_materialItemIdA offset drift");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_materialSlotB) == 0x15,
              "MSG_COMPOUND_AVATAR m_materialSlotB offset drift");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_materialItemIdB) == 0x17,
              "MSG_COMPOUND_AVATAR m_materialItemIdB offset drift");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_optionIdx) == 0x1b,
              "MSG_COMPOUND_AVATAR m_optionIdx offset drift");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_slot) == 0x1f,
              "MSG_COMPOUND_AVATAR m_slot offset drift");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_optionCheckFlag) == 0x21,
              "MSG_COMPOUND_AVATAR m_optionCheckFlag offset drift");
static_assert(offsetof(MSG_COMPOUND_AVATAR, m_type) == 0x22,
              "MSG_COMPOUND_AVATAR m_type offset drift");

#endif  // DNF_MSG_COMPOUND_AVATAR_H
