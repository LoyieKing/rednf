void CUserCharacInfo::incCurCharacMemberBonusFatigue(short value)
{
    enableSaveCharacStat();
    m_selected->m_field126c =
        (unsigned short)(m_selected->m_field126c + value);
}