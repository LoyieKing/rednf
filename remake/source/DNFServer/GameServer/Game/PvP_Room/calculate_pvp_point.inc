int PvP_Room::calculate_pvp_point()
{
    m_userTable.Clear();
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        // ORIG 085d9090：call 后 test %al;jne（无 xor/归一）；if/else 两支尾
        // + observer 空 then 共三条 jmp merge（@41/@56/@57）。
        if (P_SEATS(local_10) == 0)
            goto CP_NEXT;
        if (IsPvpObserver(local_10))
        {
            goto CP_NEXT;
        }
        if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
        {
            m_userTable.Set(local_10, P_SEATS(local_10));
            goto CP_NEXT;
        }
        else
        {
            m_userTable.Set(P_TEAMWIN(local_10), P_SEATS(local_10));
            goto CP_NEXT;
        }
CP_NEXT:
        ;
    }
    m_userTable.Calculate();
}