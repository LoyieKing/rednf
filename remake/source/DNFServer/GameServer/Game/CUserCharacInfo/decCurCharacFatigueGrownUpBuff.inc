void CUserCharacInfo::decCurCharacFatigueGrownUpBuff(unsigned int value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    if ((unsigned int)m_selected->m_field37 > value)
        m_selected->m_field37 = (unsigned short)(m_selected->m_field37 - value);
    else
        m_selected->m_field37 = 0;
    }
}