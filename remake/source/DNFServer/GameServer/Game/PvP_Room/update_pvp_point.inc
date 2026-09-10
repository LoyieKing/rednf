// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::update_pvp_point（原 TU 第 3134 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

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
}
