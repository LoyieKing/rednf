// ORIG del 0x85626aa：命中占用且指针相等者清 flag，返 1；否则返 0。
int TeamInfoThird::del(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0 && m_players[i].m_user == user)
        {
            m_players[i].m_flag = 0;
            return 1;
        }
    }
    return 0;
}