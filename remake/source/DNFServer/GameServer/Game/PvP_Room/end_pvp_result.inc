void PvP_Room::end_pvp_result()
{
    change_manager_to_winner();
    m_state = 1;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) == 0)
        {
            continue;
        }
        P_SEATS(local_10)->set_state((ch_state)6);
    }
    SkipLastRankResultTimer();
}