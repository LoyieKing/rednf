int PvP_Room::get_manager_team()
{
    if ((m_managerSeat < 0) || (7 < m_managerSeat))
    {
        return 0xfe;
    }
    return P_TEAMS(m_managerSeat);
}