bool WarField::IsGridClear()
{
    if (m_mapInfo.m_monsterMap.size() != 0)
    {
        return 0;
    }
    return 1;
}