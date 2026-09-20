void CUserCharacInfo::SetProperDungeonClearCount(short count)
{
    if (m_selected)
    {
    m_selected->m_field1236 = (unsigned short)count;
    }
}