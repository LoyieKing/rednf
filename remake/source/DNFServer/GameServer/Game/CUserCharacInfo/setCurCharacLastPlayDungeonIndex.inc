void CUserCharacInfo::setCurCharacLastPlayDungeonIndex(unsigned int index)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_lastPlayDungeonIndex = index;
    }
}