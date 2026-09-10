void TeamInfo::reset()      // ORIG T 0x0855cb16
{
    for (int i = 0; i <= 3; ++i)
        m_players[i].reset();
