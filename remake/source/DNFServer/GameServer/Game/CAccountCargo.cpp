// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CAccountCargo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CAccountCargo（账号金库，G4 商城/交易域批次）
// 参照 docs/class_func_reports/CAccountCargo.md + ORIG 反汇编逐函数核对。
// 布局（0xd68）见 CUser.h CAccountCargo 注释；55 槽 Inven_Item + 金钱/容量。
// 跨类调用以 ORIG 真实符号声明（asm-label extern）。
// ============================================================================

#include <cstring>

#include "CUser.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "CInventory.h"
#include "CTitleBook.h"
#include "DB_AccountCargoSync.h"
#include "ServiceRestrictManager.h"
#include "Arad_DataManager.h"

extern unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
extern Inven_Item g_emptySlot;  // ORIG 全局（0x943ddc0），定义于 Inven_Item.cpp（ORIG Inven_Item TU）

// ============================================================================
// 构造 / 基础存取（ORIG 0x8289794 / 0x8289816 / 0x828986c / 0x82898c0）
// ============================================================================
#include "CAccountCargo/CAccountCargo.inc"
}

#include "CAccountCargo/SetDBData.inc"
}

#include "CAccountCargo/Clear.inc"
}

#include "CAccountCargo/ResetSlot.inc"
}

#include "CAccountCargo/GetCapacity.inc"
}

#include "CAccountCargo/SetCapacity.inc"
}

#include "CAccountCargo/GetMoney.inc"
}

#include "CAccountCargo/SetStable.inc"
}

#include "CAccountCargo/IsAlter.inc"
}

// ---- 槽位访问 ----
#include "CAccountCargo/CheckValidSlot.inc"
}

#include "CAccountCargo/GetSlot.inc"
}

#include "CAccountCargo/GetSlotRef.inc"
}

// ---- 插入条件 / 插入 / 删除 / 移动 ----
#include "CAccountCargo/CheckInsertCondition.inc"
}

#include "CAccountCargo/InsertItem.inc"
}

#include "CAccountCargo/DeleteItem.inc"
}

#include "CAccountCargo/MoveItem.inc"
}

// ---- 查询 ----
#include "CAccountCargo/GetEmptySlot.inc"
}

#include "CAccountCargo/CheckSlotEmpty.inc"
}

#include "CAccountCargo/GetSpecificItemSlot.inc"
}

#include "CAccountCargo/CheckStackLimit.inc"
}

#include "CAccountCargo/GetItemCount.inc"
}

// ---- 金钱 ----
// [b28d-solo-CAccountCargo] Main 已落 CUser.h:844 int→bool（P292：ORIG 调用点
// 0x0828a1f0 段 `call; xor $0x1,%eax; test %al,%al; je`）。
#include "CAccountCargo/CheckMoneyLimit.inc"
}

#include "CAccountCargo/AddMoney.inc"
}

#include "CAccountCargo/SubMoney.inc"
}

// CUser.h:820 声明已同步为 bool（caller 0x0822508c test %al 直判证据，
// Main 代落头）；定义体返回类型随之改 bool，return 0/1 字面量语义不变。
#include "CAccountCargo/DepositMoney.inc"
}

#include "CAccountCargo/WithdrawMoney.inc"
}

// ---- 发包 ----
#include "CAccountCargo/SendNotifyMoney.inc"
}

#include "CAccountCargo/SendItemList.inc"
}

// [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828ab1c：158/158；空槽/非空块序按 ORIG
//   `test m_addInfo; je <空槽块尾部+0x196>` 反转为 `if (addInfo != 0) {非空} else {空槽}`
//   （P191/P222）。p167 CLEAR（27/27）。
#include "CAccountCargo/MakeItemPacket.inc"
}

// ---- 创建 / 升级 / 删除（DB 联动） ----
#include "CAccountCargo/CreateAccountCargo.inc"
}

#include "CAccountCargo/UpgradeAccountCargo.inc"
}

#include "CAccountCargo/SendNotifyRecipe.inc"
}

#include "CAccountCargo/DeleteAccountCargo.inc"
}
