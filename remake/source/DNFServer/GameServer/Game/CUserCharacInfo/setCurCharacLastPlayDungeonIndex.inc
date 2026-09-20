void CUserCharacInfo::setCurCharacLastPlayDungeonIndex(unsigned int index)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field126f = index;
    }
}