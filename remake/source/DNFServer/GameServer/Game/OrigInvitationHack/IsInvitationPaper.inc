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
