void CUserCharacInfo::addCurcharacStamina(unsigned char value)
{
    enableSaveCharacStat();
    CUR->m_stamina = (unsigned char)(CUR->m_stamina + value);
