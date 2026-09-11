int PvP_Room::get_player_count()
{
    int local_14 = 0;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0 && !IsPvpObserver(local_10))
        {
            local_14 = local_14 + 1;
        }
    }
    return local_14;
}