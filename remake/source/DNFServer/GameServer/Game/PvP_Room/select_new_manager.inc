int PvP_Room::select_new_manager()
{
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (P_SEATS(local_8) != m_seats[7])
            {
                m_seats[7] = P_SEATS(local_8);
                m_managerSeat = local_8;
                P_SEATSTATE(local_8) = 0;
                return 1;
            }
        }
    }
    return 0;
}