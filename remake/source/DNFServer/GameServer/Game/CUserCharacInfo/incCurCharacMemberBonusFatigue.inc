void CUserCharacInfo::incCurCharacMemberBonusFatigue(short value)
{
    enableSaveCharacStat();
    CUR->m_memberBonusFatigue =
        (unsigned short)(CUR->m_memberBonusFatigue + value);
