int CParty::join_user(CUser* user)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "party.cpp",
                              __PRETTY_FUNCTION__, 0x419,
                              "Null user");
        return -1;
    }
    if (m_state == 2)
    {
        return -2;
    }
    // [Ghidra 0x0859b2b6+setge 朝向] 满员判定写 count>=max 正向（ORIG
    // `cmp %edx,%eax; setge`）；耗尽返 0、满员/空参返 -1。
    if (get_member_count() >= static_cast<int>(m_userMax))
    {
        return -1;
    }
    for (int i = 0; i <= 3; i++)
    {
        if (m_member[i].m_pUser == 0)
        {
            m_member[i].m_pUser = user;
            m_member[i].m_field8 = static_cast<int>(user->GetUID());
            user->SetPartyIndex(static_cast<short>(m_partyIndex));
            reinterpret_cast<CUserCharacInfo*>(user)
                ->resetStraightVictories();
            return 1;
        }
    }
    return 0;
