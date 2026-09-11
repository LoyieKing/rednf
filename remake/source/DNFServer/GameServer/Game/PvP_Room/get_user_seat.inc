int PvP_Room::get_user_seat(CUser* user)
{
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == user)
            return local_8;
    }
    return -1;
}