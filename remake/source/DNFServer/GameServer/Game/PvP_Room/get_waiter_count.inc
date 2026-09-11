int PvP_Room::get_waiter_count() const
{
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            local_c = local_c + 1;
        }
    }
    return local_c;
}