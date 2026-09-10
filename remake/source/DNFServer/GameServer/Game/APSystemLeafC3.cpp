// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 APSystemLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// APSystemLeafC3.cpp — AP 域叶片批次 C3（独占 TU；CMake 由 Main 统一登记）
//
// 覆盖符号（全部经 df_game_r elf32-i386 nm 核实）：
//   W 0x0889566e _ZN8APSystem14CActionPointExC1Ev/C2Ev
//       APSystem::CActionPointEx::CActionPointEx()
//       —— 无独立报告：按规则先读同类报告（APSystem__CActionPointEtcParameter/
//          importActionPointSystemEtcScript.md，其 -0x154 栈对象即调用者实证）
//          后 objdump 逐指令转写。
//   W 0x0848f5b4 _ZN8APSystem12_ActionPointC1Ev/C2Ev
//       APSystem::_ActionPoint::_ActionPoint()（上者基类子对象构造 = 直接依赖，
//       仅补 ORIG nm 真实符号）
//       —— 无独立报告：objdump 转写（体为 memset(this,0,0xb)）。
//   T 0x08894956 _ZN8APSystem24CActionPointEtcParameter23IsValidActionGroupIndexENS_17_ActionGroupIndexE
//       APSystem::CActionPointEtcParameter::IsValidActionGroupIndex(
//           _ActionGroupIndex)
//       —— 报告 docs/class_func_reports/APSystem__CActionPointEtcParameter/
//          IsValidActionGroupIndex.md ## 汇编 + ## 反编译 C 全文已读后转写。
//
// ABI 关键证据：
//   * IsValidActionGroupIndex 为【静态成员】：函数体仅读 [ebp+8]（即组值本身，
//     范围判定 [1,0x2d]）；两处调用方（importActionPointSystemEtcScript
//     0x8894b12、DB_LoadActionPoint::ParseActionClearState 0x812405a）在 call 前
//     均只向 (%esp) 写入一个实参。据此已在 CDataManager.h 补 static 声明。
//   * CActionPointEx 权威头（APSystemTypes.h）为平铺布局；ORIG 实际带基类
//     _ActionPoint（占 +0x00..+0x0a，步长 0xb）。为不破坏既有平铺索引调用方
//     （ScriptLeafA2.cpp 按 m_bytes02[i] 直访），保持平铺布局并在本 TU 以等价
//     成员区 memset 复刻基类子对象清零；_ActionPoint 类本体已按 ORIG 真实布局
//     （pack(1)，sizeof 0xb）落入 APSystemTypes.h，ctor 符号由本 TU 提供。
// ============================================================================

#include <string.h>

#include "APSystemTypes.h"   // CActionPointEx / _ActionPoint 权威布局
#include "CDataManager.h"    // CActionPointEtcParameter 成员声明
#include "CUserProc.h"       // APSystem::_ActionGroupIndex 完整定义

namespace APSystem
{

// ----------------------------------------------------------------------------
// _ActionPoint::_ActionPoint —— ORIG W 0x0848f5b4（无独立报告，objdump 转写；
// range [0x0848f5b4,0x0848f5d7]）
//   push %ebp; mov %esp,%ebp; sub $0x18,%esp
//   movl $0xb,0x8(%esp); movl $0x0,0x4(%esp)
//   mov 0x8(%ebp),%eax; mov %eax,(%esp); call memset@plt
//   leave; ret
// ----------------------------------------------------------------------------
#include "APSystemLeafC3/_ActionPoint.inc"
}

// ----------------------------------------------------------------------------
// CActionPointEx::CActionPointEx —— ORIG W 0x0889566e（无独立报告，objdump
// 转写；调用者 importActionPointSystemEtcScript.md 栈对象 -0x154 同构实证）
//   call _ZN8APSystem12_ActionPointC1Ev      // 基类子对象 [+0x00,+0x0b)
//   movw $0x0,0xc(%eax)                      // +0x0c word = 0
//   movb $0x0,0xe/0xf/0x10(%eax)             // +0x0e/+0x0f/+0x10 = 0
//   call _ZNSt6vectorIiSaIiEEC1Ev @+0x14     // m_itemList 默认构造
//   movl $0x7,0x20(%eax)                     // m_weekday = 7
//   call _ZNSsC1Ev @+0x24 / @+0x28           // m_str24/m_str28 默认构造
// 平铺权威头无法表达基类关系（改布局会破坏 ScriptLeafA2 等按偏移直访的既有
// 调用方），基类清零以等价成员区 memset 复刻；vector/string 由编译器默认构造
// 自动完成（与 ORIG 同为成员构造调用）。已知差异：ORIG 中 +0x0c..+0x10 清零
// 位于 mem-initializer 阶段（先于成员构造），此处位于构造函数体（后于成员构
// 造）；最终对象状态一致。
// ----------------------------------------------------------------------------
#include "APSystemLeafC3/CActionPointEx.inc"
}

// ----------------------------------------------------------------------------
// IsValidActionGroupIndex —— 报告 IsValidActionGroupIndex.md 全文转写
// ORIG T 0x08894956 range [0x08894956,0x08894975]：
//   mov 0x8(%ebp),%eax; test %eax,%eax; jle L_false   // v <= 0 -> false
//   cmp $0x2d,%eax; jg L_false                        // v > 45 -> false
//   mov $0x1,%eax / L_false: mov $0x0,%eax            // else true
// 静态成员：唯一实参（_ActionGroupIndex 单 int 按值传递）即 [ebp+8]。
// ----------------------------------------------------------------------------
#include "APSystemLeafC3/IsValidActionGroupIndex.inc"
}

}  // namespace APSystem
