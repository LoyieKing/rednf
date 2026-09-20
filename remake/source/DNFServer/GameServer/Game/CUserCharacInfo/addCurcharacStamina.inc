void CUserCharacInfo::addCurcharacStamina(unsigned char value)
{
    enableSaveCharacStat();
    m_selected->m_stamina = (unsigned char)(m_selected->m_stamina + value);
}