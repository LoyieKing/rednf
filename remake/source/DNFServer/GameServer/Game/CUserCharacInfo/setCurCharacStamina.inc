void CUserCharacInfo::setCurCharacStamina(unsigned char value)
{
    enableSaveCharacStat();
    m_selected->m_stamina = value;
}