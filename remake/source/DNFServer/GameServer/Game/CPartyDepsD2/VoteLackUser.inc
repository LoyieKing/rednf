int CParty::VoteLackUser(CUser* user, int victimIdx)
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
