// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inven_Item/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Inven_Item 还原（2026-08-16）。
// 布局/函数形态以 ORIG 反汇编为准（AE 口径：调用目标归一）。

#include "Inven_Item.h"

// ORIG 全局（0x943ddc0）：空槽模板。定义于本 TU 顶部 —— ORIG 的
// _GLOBAL__I_g_emptySlot @ 0x0850d0cb 位于 Inven_Item 内联函数群首部，
// 供 CAccountCargo/CInventory 等 extern 引用。
Inven_Item g_emptySlot;

// ===================== stAmplifyOption_t（ORIG 0x80cb75c 族） =====================

#include "Inven_Item/getAbilityType.inc"


#include "Inven_Item/getAbilityValue.inc"


#include "Inven_Item/reset.inc"


#include "Inven_Item/identify.inc"


#include "Inven_Item/isIdentified.inc"


#include "Inven_Item/hasAbility.inc"


#include "Inven_Item/_setAbility.inc"


#include "Inven_Item/assign.inc"


#include "Inven_Item/SetLock.inc"


#include "Inven_Item/GetLock.inc"


#include "Inven_Item/getValues.inc"


// ===================== Inven_Item 内嵌辅助类 =====================

#include "Inven_Item/reset_1.inc"


#include "Inven_Item/reset_2.inc"


// RandomOptionSeed::reset 定义已迁 DnfItemInfo.cpp（unit-at-a-time outgoing
// 塌缩修复：本 TU 可见 callee 体使 RandomOption::reset outgoing 塌缩 0x4，
// ORIG sub $0x18；挪走后 4.4 恢复 0x18）。

#include "Inven_Item/reset_3.inc"
{
    m_slot0.reset();
    m_slot1.reset();
    m_slot2.reset();
    m_gradeSeed.reset();
    m_changedOption.reset();
    m_changedSlot.reset();
}

#include "Inven_Item/empty.inc"
{
    if (m_slot0.m_optionIndex == 0)
        return true;
    return false;
}

#include "Inven_Item/UpgradeSeparateInfo.inc"
{
    reset();
}

#include "Inven_Item/reset_4.inc"
{
    // ORIG 0x80cb346：位域 b0:5/b1:1/b2:2 清零 → 三次位清
    // （&0xffffffe0 → &0xffffffdf → &0x3f），整字节清零。
    struct Bits { unsigned char a:5, b:1, c:2; };
    reinterpret_cast<Bits*>(&m_packed)->a = 0;
    reinterpret_cast<Bits*>(&m_packed)->b = 0;
    reinterpret_cast<Bits*>(&m_packed)->c = 0;
}

#include "Inven_Item/GetUpgradeSeparate.inc"


#include "Inven_Item/SetUpgradeSeparate.inc"


#include "Inven_Item/IsTradeRestriction.inc"


#include "Inven_Item/ReservedCapacity.inc"


#include "Inven_Item/reset_5.inc"


#include "Inven_Item/Inven_Item.inc"


#include "Inven_Item/reset_6.inc"


#include "Inven_Item/set_add_info.inc"


#include "Inven_Item/get_add_info.inc"


#include "Inven_Item/ResetItemAttr.inc"


#include "Inven_Item/GetUpgrade.inc"


#include "Inven_Item/SetUpgrade.inc"


#include "Inven_Item/IncUpgrade.inc"


#include "Inven_Item/GetItemAttr.inc"


#include "Inven_Item/SetItemAttr.inc"


#include "Inven_Item/GetReSealCount.inc"


#include "Inven_Item/SetReSealCount.inc"


// [SplitC 2026-09-11 batch2 · TU 迁移] DecreaseTradeLimitCount 已迁出至 LimitOfTradeCountHelper.cpp（ORIG GI 键对齐）。



#include "Inven_Item/GetTradeLimitCount.inc"


#include "Inven_Item/SetTradeLimitCount.inc"


#include "Inven_Item/isAvatarItemType.inc"


#include "Inven_Item/IsCreatureItemType.inc"


#include "Inven_Item/isEquipableItemType.inc"


#include "Inven_Item/isEmpty.inc"


#include "Inven_Item/setCopy.inc"


#include "Inven_Item/getPackedCode1.inc"


#include "Inven_Item/getKey.inc"


#include "Inven_Item/GetInventoryType.inc"


#include "Inven_Item/GetItemSpace.inc"


#include "Inven_Item/SetUpgradeSeparateAndTradeRestriction.inc"

