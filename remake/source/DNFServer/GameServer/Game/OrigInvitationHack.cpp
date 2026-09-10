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
bool CHackLog_InvitationPaperSellToNPC::IsInvitationPaper(int itemNo)  // [b28d-Main 代落·S328 P292/P315 档①] 返回 int→bool（与 CHackLog_InvitationPaperSellToNPC.h:13 同步；ORIG 0x08619c43 调用点 `call;test %al,%al;je` 无 setne）
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
int CHackLog_InvitationPaperSellToNPC::GetHackType()
{
    return 0x322;
}

