#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 HackTickLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// HackTickLeafC3.cpp —— df_game_r 还原：hack 计数回调叶子 + 地图游玩 tick。
// 独立叶子 TU（Main 登记 CMake）；逐函数对照 ORIG df_game_r 反汇编实现。
//
// 恢复函数与证据源：
//   1. addUserHackCount149(CUser*)        ORIG T 0x830dc9d
//      报告：docs/class_func_reports/_lt_global_gt_/addUserHackCount149.md
//      （## 汇编 + ## 反编译 C 已完整读取）
//   2. addUserHackCount150(CUser*)        ORIG T 0x830dce4
//      报告：docs/class_func_reports/_lt_global_gt_/addUserHackCount150.md
//      （## 汇编 + ## 反编译 C 已完整读取）
//   3. stMapPlayInfo_t::getPlayTick()       ORIG W 0x830e7ea（ORIG 非 const）
//      【无独立报告——objdump 直接转写并标记】：
//        mov 0x4(%eax),%edx   ; this+0x04 = m_clearTick
//        mov (%eax),%eax      ; this+0x00 = m_startTick
//        sub %eax,%edx → 返回 edx-eax（无符号）
//      布局佐证：WarField.h stMapPlayInfo_t{m_startTick +0x00, m_clearTick +0x04}；
//      同类佐证：FieldSmallStructs.cpp init() 清零同四字段；调用方语义
//      CBattle_Field/onClearMap（m_clearTick=now 后取游玩时长）一致。
//
// ODR 说明：Game/CBattle_Field_helpers.cpp（未入 CMake 孤儿 TU）含本三符号重复
// 定义；本 TU 登记后不得同时登记该文件（或须先删其重复段）。

#include "WarField.h"
#include "CUser.h"

#include "HackTickLeafC3/addUserHackCount149.inc"


#include "HackTickLeafC3/addUserHackCount150.inc"


#include "HackTickLeafC3/getPlayTick.inc"

