// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCargo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CCargo（角色仓库，G4 商城/交易域批次）
// 参照 docs/class_func_reports/CCargo.md + ORIG 反汇编。
// 布局（0x14，_Charac_info +0xdaa 内嵌）见 CUserCharacInfo.h 注释；
// 物品缓冲 0x98 槽（0x2438 = 0x98 * 0x3d），由 SetInventoryMemory 绑定。
// 跨类调用以 ORIG 真实符号声明（asm-label extern）。
// ============================================================================

#include <cstdlib>
#include <cstring>

#include "CUserCharacInfo.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "CInventory.h"

// ============================================================================
// 构造 / 析构（ORIG 0x86949da / 0x86949f8，weak）
// ============================================================================
extern unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
extern Inven_Item g_emptySlot;
#include "CCargo/checkStackableLimit.inc"
#include "CCargo/CCargo.inc"
}

#include "CCargo/dtor_CCargo.inc"
}

#include "CCargo/set_parent.inc"
}

#include "CCargo/SetCapacity.inc"
}

#include "CCargo/SetInventoryMemory.inc"
}

// ---- reset @ 0x850b0c2 ----
#include "CCargo/reset.inc"
}

// ---- set_cargo @ 0x850b116 ----
#include "CCargo/set_cargo.inc"
}

// ---- get_cargo @ 0x850b164 ----
#include "CCargo/get_cargo.inc"
}

// ---- 槽位访问 ----
#include "CCargo/GetCapacity.inc"
}

#include "CCargo/GetItemCount.inc"
}

#include "CCargo/get_cargo_start.inc"
}

#include "CCargo/get_empty_slot.inc"
}

#include "CCargo/IsOperatorable.inc"
}

#include "CCargo/GetCargoRef.inc"
}

#include "CCargo/GetCargoRef_1.inc"
}


// ---- get_cargo_slot @ 0x850b2b4（按值返回 Inven_Item） ----
// ORIG 0x850b2b4：`call IsOperatorable; xor $0x1; test %al,%al; je +0xaf` ——
// je 落点 +0xaf 为 `mov this+0x4,%edx; imul $0x3d; memcpy(sret, &m_pItems[idx],0x3d)`，
// 即 **IsOperatorable==1（合法索引）→ 返回 m_pItems[idx] 原内容**；
// IsOperatorable==0 → `Inven_Item out; memset(&out,0,0x3d);` 返空 item。
// [b28d 真语义 bug 修正] 旧码分支内容互换（!IsOperatorable 返回原内容），已按 ORIG 翻转。
// [b28d-P59 修复] 否定守卫 + 尾返回成员后 68=68 **AE**（旧码 104 条）。
// 语义与 ORIG 分支一一对应（IsOperatorable==1 返 m_pItems[idx] / ==0 返空 item）。
#include "CCargo/get_cargo_slot.inc"
}

// ---- GetCargoData @ 0x850c6e0 ----
#include "CCargo/GetCargoData.inc"
}

 // ---- 插入 ----
#include "CCargo/insert_item.inc"
}

// [b28d-solo-CCargo] AE（fresh 90=90；compare_tu clean 口径）——三处翻正：
// ①`if (!checkStackableLimit(...)) return -10;` 否定守卫（P168a① 产
// `call; xor $0x1; test; je <成功直落>`）；②尾块序 `if (==) {…; return slot;} return -1;`
// （P222；ORIG else 块在函数尾直落）；③P48 最后 += 加数独立重调。
// 原 N18b（cmp 方向）+ 帧 0x10/0x20 残差随块序翻正一并消失。
#include "CCargo/_AddStackItem.inc"
}

// [b28d-solo-CCargo] AE（fresh 65=65；compare_tu clean 口径）——尾块序翻正：
// ORIG `jne <ret -1 块，物理最末直落 epilogue>` + `mov 0x10(%ebp),%eax; jmp` 在前
// ⇒ 源码 `if (==) {…; return slot;} return -1;`（P222 正向包裹）。原 N18b 判词作废。
#include "CCargo/_tryAddStackItem.inc"
}

// [b28d-solo-CCargo] AE（fresh 82=82；compare_tu clean 口径）——①if/else 交换：
// ORIG 循环分支（ret2@-0x10, i@-0x14）物理在前、slot 分支（ret1@-0xc）在后
// （`cmp; je +0xac`）；②尾段 `if (addInfo==0) return slot; return -1;` 复现 ORIG
// `test; jne <ret -1 块>` + slot 块 jmp / -1 块直落。原 N82 槽平移判词作废。
#include "CCargo/tryInsertItemSpecialSlot.inc"
}

#include "CCargo/insert_item_special_slot.inc"
}

// ---- 删除 ----
// [b28d-solo-CCargo] AE（fresh 116=116；compare_tu clean 口径）——①isEquipableItemType
// 正极性 bool 守卫（ORIG `test %al,%al; je`，无 xor/setne）+ 分支交换（装备路径在前）；
// ②删具名 cur 局部（P0/P48：两次独立重读 m_addInfo2，`<=0` 对应 ORIG jle）；
// ③goto 尾标签串 ZERO/ONE 复现 `jle <mov $0; jmp>; mov $1 直落` 尾块序（P223）。
// 原 N82 判词作废。
#include "CCargo/delete_item.inc"
}

// ---- _ResetSlot @ 0x850b1a0 ----
#include "CCargo/_ResetSlot.inc"
}

#include "CCargo/move_item.inc"
}

#include "CCargo/update_item.inc"
}

// [b28d-P210②] ORIG 0x0850c4ba 位于全部 caller 之后（insert_item/update_item/
// tryInsertItemSpecialSlot 均在前）⇒ 4.4 对「未见定义」的 callee 用保守 outgoing
// 帧（sub $0x18，与 ORIG 一致）；定义前置会使 caller 帧塌缩为 0xc。
#include "CCargo/_AddItem.inc"
}
// ---- 查询 ----
#include "CCargo/check_slot_empty.inc"
}

#include "CCargo/check_item_exist.inc"
}

#include "CCargo/_GetItemCount.inc"
}

#include "CCargo/calcCargoItemCount.inc"
}

#include "CCargo/_IsExistEquipItem.inc"
}

#include "CCargo/FindItemLock.inc"
}

// ORIG 0x850c656：m_capacity==0 或 m_capacity>capacity 拒绝；仅 m_capacity<capacity
// 升级（==拒绝）。ORIG 汇编 test/je→mov$0 + Ghidra C 双证，旧注释语义相反。
#include "CCargo/UpgradeCargo.inc"
}

// ORIG 0x850c698：同上判定，capacity<=0x98 才可升。
// [b28d-N] 真语义 bug 修正：ORIG 首段 `test %eax,%eax; je +0x18(mov$0)` ⇒
// m_capacity==0 直接 return 0（与 `m_capacity>capacity` 共享 ret0 块）。
// 旧写 `m_capacity != 0 && m_capacity > capacity` 在 capacity∈[1,0x98] 时误返 1；
// 短路形 `==0 || >` 同时复现 ORIG 的 je/fall 共享块（P214①/P59）。
#include "CCargo/isUpgradable.inc"
}

// [b28d-solo-CCargo] AE（fresh 60=60；compare_tu clean 口径）——双 goto 尾标签串复现
// ORIG `je +0xaa`（ta==9999）/`je +0xa3`（addInfo 相等）两 return 0 出口块
// （+0xa3 在 +0xaa 前）；ta/tb 有符号序 + addInfo 无符号序（P196）。残差仅 .o 短地址显示。
#include "CCargo/_CompareSlot.inc"
}

#include "CCargo/sort.inc"
}

// ---- 发包 ----
// [b28d-solo-CCargo] AE（fresh 114=114；compare_tu clean 口径）——①删 buf 指针局部
// （P183 地址重物化：ORIG 每处 `lea -0x1c(%ebp),%eax` 重发）；②put_short(count)
// 具名重读（P244：ORIG `mov -0x10(%ebp),%eax` 非立即数 0）；③GetLock 内联实参
// （P200：`call; movzbl %al,%ebx` 跨 GetCharacExpandDataR 保活；具名/register 两形
// 实测多槽或两段零扩展）。原 N82 判词作废。
#include "CCargo/SendItemLockList.inc"
}

// [b28d-solo-CCargo] N 封顶（fresh 184/185；唯余 1 条 mov 中转）——已翻正：
// ①删 buf 指针局部（P183）；②删 world 局部 + IsEnchantRevisionChannel 正极性
// （ORIG `call; test %al,%al; je <m_fieldd>`）；③GetItemCount 保持 int（LHS-first 直存）。
// 残差 = GetCapacity 调用点：ORIG `mov 0xc(%ebp),%edx; mov %eax,0x4(%esp); mov %edx,(%esp)`
// vs 4.4 `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0x4(%esp); mov %eax,(%esp)`。
// 归因 P248 档①两端冲突：callee 返回类型是唯一旋钮——改 int 可翻正此点（同函数
// GetItemCount int 处实测 LHS-first；unsigned 化后两处同变 RHS-first，已回退），但
// CUser.cpp:10586 `sig->m_cargoCapacity = GetCapacity()`（SigTypes m_cargoCapacity=
// unsigned int，+0xa8e0）需 unsigned 才同型直存（b28d-solo-CUser 定案）⇒ 禁单段翻正。
// 实测 (int) / (int)(unsigned int) / P226 语句表达式三形均无效。语义全等，byte 层 N 封顶。
#include "CCargo/MakeItemList.inc"
}

// [b28d-solo-CCargo] AE（fresh 190=190；compare_tu clean 口径）——if/else 交换：
// ORIG `test %eax,%eax; je <+0x1e0 空槽块>`（非空块直落、空槽块在函数尾 `jmp +0x29e`）
// ⇒ 源码 `if (m_addInfo != 0) {非空} else {空槽}`。p167 调用点差集 CLEAR（28=28）。
// 原 b21 verdict=DIFF / N82 判词作废（块序翻正后槽号自然对齐）。
#include "CCargo/MakeItemPacket.inc"
}
