void CUserCharacInfo::incCurCharacUsedFatigue(unsigned short value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_usedFatigue = (unsigned short)(CUR->m_usedFatigue + value);
    }
}