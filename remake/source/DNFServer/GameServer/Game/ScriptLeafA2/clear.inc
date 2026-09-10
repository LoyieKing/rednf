void stNPCCommonData_t::clear()
{
    for (int i = 0; i <= 9; ++i)
    {
        m_favorExp[i] = 0;
    }
    m_maxFavorExp = 0;
    m_maxFavorLevelNpc = 0;
    m_favorSystemBeginQuest = 0;
    m_favorSystemBeginLevel = 0;
