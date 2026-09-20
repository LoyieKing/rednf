void CUserCharacInfo::resetCharacFatigueGrownUpBuff()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field37 = 0;
    }
}