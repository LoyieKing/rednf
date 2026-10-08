// ORIG 0x081dff32（报告 Dispatcher_TournamentRewardSelectState/check_error.md）：
// state!=5 → 0x7fffffff；GetParty(GetPartyIndex)==0 → -1；
// TournamentDungeonClearState!=1 → 0x7fffffff；!CheckRecvFromAllMember → 0x7fffffff。
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x081dff32
//   [N82→P200] 根因=具名 int partyIdx 落 -0x10 槽（帧 0x24→0x28）+ 缺 user==0 判空；
//   改 GetPartyIndex() 内联实参（movswl→%ebx 跨 call）+ 补判空 ⇒ 归零。
int Dispatcher_TournamentRewardSelectState::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    // [b28d-solo-S206] ORIG 0x081dff32 首条 `cmpl $0x0,0xc(%ebp); jne; mov $0x7fffffff`
    //   （user==0 判空）。
    // [DNF-FIX 2026-10-08 c27] ORIG 0x081dff3f user==0 → `mov $0xffffffff,%eax`（-1），非 0x7fffffff。
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 5)
    {
        return 0x7fffffff;
    }
    // [b28d-solo-S206] P200：ORIG 0x081dff4x `call GetPartyIndex; movswl %ax,%ebx;
    //   call G_CGameManager; … mov %ebx,0x4(%esp)`（短整拓宽直传实参、跨 callee-saved
    //   保活，无栈槽）。具名 int 局部 partyIdx 使 4.4 落 -0x10 槽并帧 0x24→0x28。
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return -1;
    }
    if (party->GetTournamentDungeonClearState() != 1)
    {
        return 0x7fffffff;
    }
    if (!party->CheckRecvFromAllMember(user))
    {
        return 0x7fffffff;
    }
    return 0;
}