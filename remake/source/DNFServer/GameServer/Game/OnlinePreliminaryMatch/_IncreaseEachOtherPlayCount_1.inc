void COnlinePreliminary::_IncreaseEachOtherPlayCount()
{
    /* 同 _GetEachOtherPlayCount()：共享函数级 i【ORIG 085605ee @+0x14/@+0x1b
     * 双写同槽】 */
    CUser* lastA = 0;
    CUser* lastB = 0;
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (m_teams[0].m_seats[i].m_flag != 0 &&
            m_teams[0].m_seats[i].m_user != 0)
            lastA = m_teams[0].m_seats[i].m_user;
    }
    for (i = 0; i <= 3; ++i)
    {
        if (m_teams[1].m_seats[i].m_flag != 0 &&
            m_teams[1].m_seats[i].m_user != 0)
            lastB = m_teams[1].m_seats[i].m_user;
    }
    /* ORIG @0856069f/0x6a5 双 je 直指 epilogue 无空块岛：正向 && 嵌套
     * 【技巧11】 */
    if (lastA != 0 && lastB != 0)
    {
        _IncreaseEachOtherPlayCount(lastA, lastB);
        _IncreaseEachOtherPlayCount(lastB, lastA);
    }
