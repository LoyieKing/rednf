void CMissionList_Charac::_reset()
{
    std::memset(m_missionList, 0, sizeof(m_missionList));
    m_clearedFlags.reset();
    m_field148 = 0;
    m_field14a = 0;
    m_field14c = 0;
    m_field14e = 0;
    m_user = 0;
}