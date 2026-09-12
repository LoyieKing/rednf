// ---------------------------------------------------------------------------
// CParty::VoteLackUser —— ORIG T 0x085b3c90（报告 VoteLackUser.md）：
//   缺人投票（cElection<int,4,4>），投票成功后 0xf 定时器 0x9c4ms。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（91=91）：P221 全表达式临时链翻 sibling 块 trace 槽
// （ORIG -0x30/-0x20 vs 具名形 -0x24/-0x34）+ P24 gen_timer_key 内联实参（消 -0xc
// 槽，key 升格 %ebx 跨 G_TimerQueue call）。语义/调用集齐。
// [R11fixE7 2026-09-12] int→bool：ORIG prett 串 `bool CParty::VoteLackUser(CUser*,
// int)`（raw @0x8cbb440，P174 直证）；体内 `return 0/1`。
bool CParty::VoteLackUser(CUser* user, int victimIdx)
{
    if (victimIdx < 0 || victimIdx > 3)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3591, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x3591);
        return 0;
    }
    CUser* victim = get_user(victimIdx);
    if (user == victim || victim == 0)
        return 0;
    int voterSlot = (int)GetMemberSlotNo(user);
    if (voterSlot < 0 || voterSlot > 3)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x35a1, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x35a1);
        return 0;
    }
    m_election.Vote(voterSlot, victimIdx);
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0xf, (long long)0x9c4,
        (int)gen_timer_key((TIMER_MESSAGE)0xf), 0);
    return 1;
}