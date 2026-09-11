unsigned int COnlinePreliminary::_GetEachOtherPlayCount()
{
    /* ORIG @085605f4 槽序 lastA=-0x14/lastB=-0x10/i=-0xc；两循环共享同一
     * 函数级 i（@+0x22 与 @+0x74 两次 i=0 初始化同一槽）*/
    CUser* lastA = 0;
    CUser* lastB = 0;
    int i = 0;                                     /* @08560522 声明+for 双写 */
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
    /* ORIG @085605bf/0x5c5 两独立 je→mov $0 岛(@+0x8f)：|| + goto 合并双直
     * 跳，消除 4.4 单 goto 的跳板块【技巧23 变体】 */
    if (lastA == 0 || lastB == 0)
        goto RET0;
    return _GetEachOtherPlayCount(lastA, lastB);
RET0:
    return 0;
}