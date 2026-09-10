// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OrigInvitationHack/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// OrigInvitationHack.cpp — CHackLog_InvitationPaperSellToNPC 域（2 个 ORIG 强符号）
//
// 报告来源（逐份转写，汇编+反编译 C）：
//   docs/class_func_reports/CHackLog_InvitationPaperSellToNPC/
//     GetHackType       T 0x08286714 _ZN33CHackLog_InvitationPaperSellToNPC11GetHackTypeEv
//     IsInvitationPaper T 0x082866d0 _ZN33CHackLog_InvitationPaperSellToNPC17IsInvitationPaperEi
//
// 类声明权威头：CHackLog_InvitationPaperSellToNPC.h（唯一类声明，本 TU 唯一实现）。
// 两函数均不使用 this，但 ORIG mangled 为普通成员符号，故按成员函数定义。
// 静态数组 INVITATION_PAPER_NO 对应 ORIG .rodata 局部符号 _ZL19INVITATION_PAPER_NO
// @0x08c101c0（7 项，段换算 dump 实测：0xc8d 0xc8e 0xc8f 0xc93 0xcb8 0xcd0 0xcd2）。
// ============================================================================

#include "CHackLog_InvitationPaperSellToNPC.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit；IsInvitationPaper
// 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN33CHackLog_InvitationPaperSellToNPC17IsInvitationPaperEi`（ORIG 0x828675e）。
// 零语义/布局改动。
#include <iostream>

// ORIG _ZL19INVITATION_PAPER_NO @0x08c101c0（.rodata，7 x int）
const int INVITATION_PAPER_NO[7] = { 0xc8d, 0xc8e, 0xc8f, 0xc93, 0xcb8, 0xcd0, 0xcd2 };

// ORIG 0x082866d0：i 0..6 遍历 INVITATION_PAPER_NO，命中返 1，否则返 0
#include "OrigInvitationHack/IsInvitationPaper.inc"
}

// ORIG 0x08286714：恒返 0x322
#include "OrigInvitationHack/GetHackType.inc"
}

