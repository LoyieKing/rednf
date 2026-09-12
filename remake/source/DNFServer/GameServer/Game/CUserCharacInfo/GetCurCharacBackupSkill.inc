const char* CUserCharacInfo::GetCurCharacBackupSkill() const
{
    return m_selected ? (const char*)&CUR->m_backupSkill : 0;
}