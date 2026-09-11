void PvP_Room::send_to_pvp(PacketGuard& guard)
{
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0)
        {
            P_SEATS(local_10)->Send(guard);
        }
    }
}