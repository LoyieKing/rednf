bool CMissionList_Charac::empty_progress() const
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        if (m_missionList[kind].m_index != 0)
        {
            return 0;
        }
    }
    return 1;
}