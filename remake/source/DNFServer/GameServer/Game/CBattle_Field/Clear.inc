void CBattle_Field::TournamentInfo::Clear()
{
    m_roundOrder = -1;
    for (int i = 0; i <= 1; ++i)
    {
        m_monster[i].Clear();
    }
    m_group = 0;
