// ORIG 0x0860abde（报告 CSpecialItemRoutingManager/ProcessTimeOut.md）：
// IsRoutingState()→GetFirstValidMemberState()==5/0xc 时 SetRoutingTimerOver+
// HasAllMemberSelection→processRouting；否则 SetEndRouting。恒返回 1。
// b28c-L20: P0 清绑（原 register int state asm("eax") 违反全域禁令）——ORIG 对
// GetFirstValidMemberState 只发一次 call 且结果无栈槽往返（movsbl %al,%eax;
// cmp $5,%eax; je; cmp $0xc,%eax; je + 两处空 case nop 岛），即原码为
// switch(expr) 双 case 共享块，非具名局部。
// b28c-L20: 清绑后 switch(default 前置)形复现决策树与单次 call。
// [b28d-58] DIFF N（P223 族，空 case 岛）：ORIG 0x0860abde 两个 case 块为
//   4.1 空 case 布局（case5 = `nop; jmp +0x58`、case0xc = `nop` 直落 body）；
//   4.4 对 switch 空 case 恒 jump-thread 成双 `je body`（实测三形：独立
//   break / 共享 goto 标签 / 双 goto 串——岛数恒 0）⇒ 2 nop+1 jmp 封顶，
//   语义等价（gap=3）。
int CSpecialItemRoutingManager::ProcessTimeOut(CParty* party)
{
    if (!party->IsRoutingState())
    {
        party->SetEndRouting();
        return 1;
    }
    switch (party->GetFirstValidMemberState())
    {
    default:
        party->SetEndRouting();
        return 1;
    case 5:
        break;
    case 0xc:
        break;
    }
    party->SetRoutingTimerOver();
    if (party->HasAllMemberSelection())
        processRouting(party);
    return 1;
}