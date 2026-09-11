void CUserCharacInfo::setCurCharacBackupStamina(unsigned char value)
{
    if (m_selected)
    {
    CUR->m_backupStamina = value;
    }
}