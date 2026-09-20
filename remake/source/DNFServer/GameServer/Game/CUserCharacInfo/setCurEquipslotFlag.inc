void CUserCharacInfo::setCurEquipslotFlag(char flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field1275 = flag;
    }
}