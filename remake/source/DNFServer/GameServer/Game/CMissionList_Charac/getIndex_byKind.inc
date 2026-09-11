short CMissionList_Charac::getIndex_byKind(int kind) const
{
    if ((kind < 1) || (kind > 0x23))
    {
        return 0;
    }
    return m_missionList[kind].m_index;
}