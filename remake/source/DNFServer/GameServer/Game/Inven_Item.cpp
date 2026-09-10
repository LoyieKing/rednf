// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Inven_Item/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "Inven_Item/getAbilityValue.inc"
}

#include "Inven_Item/reset.inc"
}

// ORIG W 0x0822b3d8：返回 bool = !isIdentified()（已识别→false 不改动）。
#include "Inven_Item/identify.inc"
}

#include "Inven_Item/isIdentified.inc"
}

#include "Inven_Item/hasAbility.inc"
}

#include "Inven_Item/_setAbility.inc"
}

#include "Inven_Item/assign.inc"
}

#include "Inven_Item/SetLock.inc"
}

#include "Inven_Item/GetLock.inc"
}

#include "Inven_Item/getValues.inc"
}

// ===================== Inven_Item 内嵌辅助类 =====================

#include "Inven_Item/reset_1.inc"
}

#include "Inven_Item/reset_2.inc"
}

// RandomOptionSeed::reset 定义已迁 DnfItemInfo.cpp（unit-at-a-time outgoing
// 塌缩修复：本 TU 可见 callee 体使 RandomOption::reset outgoing 塌缩 0x4，
// ORIG sub $0x18；挪走后 4.4 恢复 0x18）。

#include "Inven_Item/reset_3.inc"
}

// ORIG 0x822abd0 W：movzbl (this) → 首字节==0 返回 true。
#include "Inven_Item/empty.inc"
}

// ORIG W 0x080cb332：构造后调 reset()（位域清零）。
#include "Inven_Item/UpgradeSeparateInfo.inc"
}

#include "Inven_Item/reset_4.inc"
}

#include "Inven_Item/GetUpgradeSeparate.inc"
}

#include "Inven_Item/SetUpgradeSeparate.inc"
}

// ORIG 0x8110b0a：8 条形 = 位域 m_flag:1 读取 + unsigned char 返回（probe u1 全等）。
#include "Inven_Item/IsTradeRestriction.inc"
}

#include "Inven_Item/ReservedCapacity.inc"
}

#include "Inven_Item/reset_5.inc"
}

// ===================== Inven_Item =====================
#include "Inven_Item/Inven_Item.inc"
}

#include "Inven_Item/reset_6.inc"
}

#include "Inven_Item/set_add_info.inc"
}

#include "Inven_Item/get_add_info.inc"
}

#include "Inven_Item/ResetItemAttr.inc"
}

#include "Inven_Item/GetUpgrade.inc"
}

#include "Inven_Item/SetUpgrade.inc"
}

#include "Inven_Item/IncUpgrade.inc"
}

#include "Inven_Item/GetItemAttr.inc"
}

#include "Inven_Item/SetItemAttr.inc"
}

#include "Inven_Item/GetReSealCount.inc"
}

#include "Inven_Item/SetReSealCount.inc"
}

// ORIG _Z23DecreaseTradeLimitCountR10Inven_Item T 0x0812013c：
// ORIG 块序 jne 进主流程/false 块直落 = 早退形（P16）。
// unit-at-a-time 规避（实证 /tmp/ii_half7.cpp）：本函数须置于
// GetTradeLimitCount/SetTradeLimitCount 外联定义之前，否则 4.4 删
// framePad 槽塌缩帧 0x28→0x18、count 槽 -0x9→-0x1。
#include "Inven_Item/DecreaseTradeLimitCount.inc"
}

#include "Inven_Item/GetTradeLimitCount.inc"
}

#include "Inven_Item/SetTradeLimitCount.inc"
}

#include "Inven_Item/isAvatarItemType.inc"
}

#include "Inven_Item/IsCreatureItemType.inc"
}

#include "Inven_Item/isEquipableItemType.inc"
}

#include "Inven_Item/isEmpty.inc"
}

#include "Inven_Item/setCopy.inc"
}

#include "Inven_Item/getPackedCode1.inc"
}

#include "Inven_Item/getKey.inc"
}

#include "Inven_Item/GetInventoryType.inc"
}

#include "Inven_Item/GetItemSpace.inc"
}

#include "Inven_Item/SetUpgradeSeparateAndTradeRestriction.inc"
}
