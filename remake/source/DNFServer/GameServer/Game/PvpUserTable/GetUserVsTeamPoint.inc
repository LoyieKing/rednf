int PvpUserTable::GetUserVsTeamPoint(int userIdx, int myTeam, int enemyTeam,
                                     CUser* user)
{
    CUser* me = m_users[myTeam][userIdx].user;
    if (me == 0)
    {
        return 0;
    }
    for (int i = 0; i < 8; ++i)
    {
        CUser* other = m_users[enemyTeam][i].user;
        // ORIG 0x085dfe80 体尾 `jmp +0xaa; nop` ⇒ P223 goto 尾空标签形。
        if (other == 0)
            goto next;
        m_teamPoint[myTeam] +=
            GetExpectVal(((CUserCharacInfo*)me)->get_pvp_grade(),
                         ((CUserCharacInfo*)other)->get_pvp_grade(), user);
    next:;
    }
    return 1;
}