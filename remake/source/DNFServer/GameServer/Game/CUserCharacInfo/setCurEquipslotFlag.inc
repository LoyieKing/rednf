void CUserCharacInfo::setCurEquipslotFlag(char flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_equipslotFlag = flag;
    }
}