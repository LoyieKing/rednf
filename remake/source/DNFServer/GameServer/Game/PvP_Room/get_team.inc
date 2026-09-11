int PvP_Room::get_team(int seat)
{
    if ((seat < 0) || (7 < seat))
    {
        return 0xfe;
    }
    return P_TEAMS(seat);
}