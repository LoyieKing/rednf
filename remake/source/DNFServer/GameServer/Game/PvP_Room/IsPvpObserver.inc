bool PvP_Room::IsPvpObserver(int seat)
{
    if (seat > 7)
    {
        return 0;
    }
    if ((m_teams[seat] == 3) || (m_teams[seat] == 4))
    {
        return 1;
    }
    return 0;
}