// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081dfca2 orig/ours=58/64 ndiff-blocks=8
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%edx | mov    %eax,0x4(%edx)
int Dispatcher_TournamentRewardSelectState::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081dfca2
    stTournamentRewardSelectState& p = static_cast<stTournamentRewardSelectState&>(param);
    // ORIG 0x081dfcc8: `mov -0x10(%ebp),%edx; mov %eax,0x4(%edx)` / 0x081dfcce
    //   `mov -0x10(%ebp),%eax; mov 0x4(%eax),%eax` —— 走具名成员 p.m_4 常量偏移；
    //   `*(int*)((char*)&p+4)` 会产 `lea 0x4(%eax),%ebx` 形（我方原为 3 处 lea）。
    p.m_4 = check_error(user, msg);
    if (p.m_4 >= 1)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x52af,
                        __PRETTY_FUNCTION__,
                        p.m_4, 0);
    }
    // ORIG 0x081dfd14: `call GetPartyIndex; movswl %ax,%ebx` 后直接 `call G_CGameManager`，
    //   无 partyIndex 具名槽（我方原 `int partyIndex` 占 -0x10 槽，多 2 条指令）。
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return LineFunc(0x52b4,
                        __PRETTY_FUNCTION__,
                        0, (unsigned int)user->get_acc_id());
    }
    return 0;
}