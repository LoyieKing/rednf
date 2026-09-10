void CUserCharacInfo::setCurCharacStamina(unsigned char value)
{
    enableSaveCharacStat();
    CUR->m_stamina = value;
