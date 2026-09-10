// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W7_UserLinkTail/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CUser.h"
#include "CParty.h"

// ---------------------------------------------------------------------------
// CUser::SetChildrensDayEventValue(int slot, int targetItem, int period)
// W 0x083281be。slot>9 直接返回；否则 state@(this+0x8e098) |= 1<<slot，
// 写 targetItem 表 @(slot+0x23824)*4+0xc 与 period 表 @(slot+0x23830)*4+4。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/SetChildrensDayEventValue.inc"

// ---------------------------------------------------------------------------
// CUser::IsOKChildrensDayEventCondition() const 语义（ORIG 无 const，按头）
// W 0x08328220：state 字节 == 0xff ⇒ true。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/IsOKChildrensDayEventCondition.inc"

// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemSlot(int idx) W 0x08328240
// idx>9 → -1；否则读 slots 表 @0x8e09c + idx*4。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetChildrensDayEventTargetItemSlot.inc"

// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemPeriod(int idx) W 0x08328262
// idx>9 → -1；否则读 periods 表 @0x8e0c4 + idx*4。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetChildrensDayEventTargetItemPeriod.inc"

// ---------------------------------------------------------------------------
// CUser::GetPowerWarPartyVictory() T 0x0868856c
// 队伍存在用队伍连胜，否则角色连胜（报告转写）。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetPowerWarPartyVictory.inc"

// ---------------------------------------------------------------------------
// CUser::GetUserListByParty(std::vector<CUser*>& out) T 0x0868056a
// push self；有队伍时按槽 0..3 补齐他人（跳过空/自身），返回队伍指针。
// ---------------------------------------------------------------------------
#include "W7_UserLinkTail/GetUserListByParty.inc"
