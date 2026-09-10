// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OrigInvitationHack/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
{
    // ORIG：上限 7 存栈槽（-0x8(%ebp)），mov i,%eax / cmp 上限,%eax / setb
    // （无符号比较，Ghidra 两局部均为 uint）。
    unsigned int count = 7;
    for (unsigned int i = 0; i < count; ++i)
    {
        if (INVITATION_PAPER_NO[i] == itemNo)
        {
            return true;
        }
    }
    return false;
}

// ORIG 0x08286714：恒返 0x322
#include "OrigInvitationHack/GetHackType.inc"
{
    return 0x322;
}

