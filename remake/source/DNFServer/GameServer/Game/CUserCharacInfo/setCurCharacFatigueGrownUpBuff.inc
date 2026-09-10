void CUserCharacInfo::setCurCharacFatigueGrownUpBuff(unsigned short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        CUR->m_fatigueGrownUpBuff = value;
    }
