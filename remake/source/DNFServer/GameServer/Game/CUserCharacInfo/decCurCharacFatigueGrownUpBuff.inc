void CUserCharacInfo::decCurCharacFatigueGrownUpBuff(unsigned int value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    if ((unsigned int)m_selected->m_fatigueGrownUpBuff > value)
        m_selected->m_fatigueGrownUpBuff = (unsigned short)(m_selected->m_fatigueGrownUpBuff - value);
    else
        m_selected->m_fatigueGrownUpBuff = 0;
    }
}