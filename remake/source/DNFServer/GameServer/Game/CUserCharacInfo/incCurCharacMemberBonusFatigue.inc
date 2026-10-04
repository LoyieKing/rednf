void CUserCharacInfo::incCurCharacMemberBonusFatigue(short value)
{
    enableSaveCharacStat();
    m_selected->m_memberBonusFatigue =
        (unsigned short)(m_selected->m_memberBonusFatigue + value);
}