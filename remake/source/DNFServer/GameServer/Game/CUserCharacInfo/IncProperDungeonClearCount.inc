void CUserCharacInfo::IncProperDungeonClearCount()
{
    if (m_selected)
    {
    ++m_selected->m_properDungeonClearCount;
    }
}