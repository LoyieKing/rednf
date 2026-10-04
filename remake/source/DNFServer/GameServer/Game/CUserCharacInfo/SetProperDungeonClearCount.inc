void CUserCharacInfo::SetProperDungeonClearCount(short count)
{
    if (m_selected)
    {
    m_selected->m_properDungeonClearCount = (unsigned short)count;
    }
}