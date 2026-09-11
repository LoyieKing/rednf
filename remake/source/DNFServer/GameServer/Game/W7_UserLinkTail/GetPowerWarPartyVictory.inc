// ---------------------------------------------------------------------------
// CUser::GetPowerWarPartyVictory() T 0x0868856c
// 队伍存在用队伍连胜，否则角色连胜（报告转写）。
// ---------------------------------------------------------------------------
int CUser::GetPowerWarPartyVictory()
{
    CParty* party = reinterpret_cast<CParty*>(GetParty());
    if (party == 0)
        return getStraightVictories();
    return party->getStraightVictories();
}
