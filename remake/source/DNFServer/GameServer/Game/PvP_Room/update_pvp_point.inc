int PvP_Room::update_pvp_point()
{
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if (G_GameWorld()->IsFreePvPChannel() != 0)
        {
            for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
            {
                if (P_SEATS(local_18) == 0)
                    continue;
                if (IsPvpObserver(local_18) != false)
                    continue;
                P_SEATS(local_18)->update_old_pvp_point();
            }
            goto LAB_085dca42;
        }
    }
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        if (P_SEATS(local_14) == 0)
            continue;
        if (IsPvpObserver(local_14) != false)
            continue;
        {
            P_SEATS(local_14)->update_old_pvp_point();
            if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
            {
                P_SEATS(local_14)->update_pvp_point(
                    m_userTable.GetResult(local_14, false,
                                          P_SEATS(local_14)->get_pvp_grade()));
            }
            else
            {
                bool mid = false;                       // ORIG -0x9 byte 槽
                if (P_SEATS(local_14)->get_pvp_grade() > 10)
                    mid = true;
                P_SEATS(local_14)->update_pvp_point(
                    m_userTable.GetResult(P_TEAMWIN(local_14), mid,
                                          P_SEATS(local_14)->get_pvp_grade()));
            }
        }
    }
LAB_085dca42:
    ;
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}