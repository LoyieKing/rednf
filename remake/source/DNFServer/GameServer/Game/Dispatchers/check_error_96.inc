// ==== Dispatcher_TournamentRewardSelect::check_error (ORIG 0x081e046c) ====
int Dispatcher_TournamentRewardSelect::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 5)
    {
        return 0x7fffffff;
    }
    // GetPartyIndex 返回 short（ORIG movswl %ax 后入栈）
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return -1;
    }
    // ORIG cmp $0x2,%al：clear state == 2（选卡阶段）→ 0，否则 0x7fffffff
    if (party->GetTournamentDungeonClearState() != 2)
    {
        return 0x7fffffff;
    }
    return 0;
}