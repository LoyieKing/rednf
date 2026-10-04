void CUserCharacInfo::resetCharacFatigueGrownUpBuff()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_fatigueGrownUpBuff = 0;
    }
}