void CUserCharacInfo::resetCharacFatigueGrownUpBuff()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_fatigueGrownUpBuff = 0;
    }
