void CUserCharacInfo::setCurCharacBackupStamina(unsigned char value)
{
    if (m_selected)
    {
    m_selected->m_pad76 = value;
    }
}