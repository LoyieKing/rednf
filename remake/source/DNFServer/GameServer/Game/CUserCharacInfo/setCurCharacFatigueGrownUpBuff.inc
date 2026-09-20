void CUserCharacInfo::setCurCharacFatigueGrownUpBuff(unsigned short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        m_selected->m_field37 = value;
    }
}