// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W7_UserLinkTail/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ===========================================================================
// W7_UserLinkTail.cpp —— W7_BuildHarmonize 链接收敛批次：ChildrensDay /
// PowerWarPartyVictory / GetUserListByParty 六个 ORIG 真实符号的落点 TU。
//
// 证据：
//   _ZN5CUser25SetChildrensDayEventValueEiii    W 0x083281be（objdump 逐指令转写）
//   _ZN5CUser30IsOKChildrensDayEventConditionEv W 0x08328220
//   _ZN5CUser34GetChildrensDayEventTargetItemSlotEi   W 0x08328240
//   _ZN5CUser36GetChildrensDayEventTargetItemPeriodEi W 0x08328262
//   _ZN5CUser22GetPowerWarPartyVictoryEv        T 0x0868856c
//       （报告 docs/class_func_reports/CUser/GetPowerWarPartyVictory.md）
//   _ZN5CUser17GetUserListByPartyERSt6vectorIPS_SaIS1_EE T 0x0868056a
//       （报告 docs/class_func_reports/CUser/GetUserListByParty.md）
// 布局注释：childrensDayEventState @0x8e098（byte 位集）；target item slot 表
//   @0x8e09c、period 表 @0x8e0c4（各 int[10]，slot>9 → -1）。
// ===========================================================================

#include "CUser.h"
#include "CParty.h"

// ---------------------------------------------------------------------------
// CUser::SetChildrensDayEventValue(int slot, int targetItem, int period)
// W 0x083281be。slot>9 直接返回；否则 state@(this+0x8e098) |= 1<<slot，
// 写 targetItem 表 @(slot+0x23824)*4+0xc 与 period 表 @(slot+0x23830)*4+4。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/SetChildrensDayEventValue.inc"
}

// ---------------------------------------------------------------------------
// CUser::IsOKChildrensDayEventCondition() const 语义（ORIG 无 const，按头）
// W 0x08328220：state 字节 == 0xff ⇒ true。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/IsOKChildrensDayEventCondition.inc"
}

// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemSlot(int idx) W 0x08328240
// idx>9 → -1；否则读 slots 表 @0x8e09c + idx*4。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetChildrensDayEventTargetItemSlot.inc"
}

// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemPeriod(int idx) W 0x08328262
// idx>9 → -1；否则读 periods 表 @0x8e0c4 + idx*4。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetChildrensDayEventTargetItemPeriod.inc"
}

// ---------------------------------------------------------------------------
// CUser::GetPowerWarPartyVictory() T 0x0868856c
// 队伍存在用队伍连胜，否则角色连胜（报告转写）。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetPowerWarPartyVictory.inc"
}

// ---------------------------------------------------------------------------
// CUser::GetUserListByParty(std::vector<CUser*>& out) T 0x0868056a
// push self；有队伍时按槽 0..3 补齐他人（跳过空/自身），返回队伍指针。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetUserListByParty.inc"
}
