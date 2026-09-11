void CUserCharacInfo::SetProperDungeonClearCount(short count)
{
    if (m_selected)
    {
    CUR->m_properDungeonClearCount = (unsigned short)count;
    }
}