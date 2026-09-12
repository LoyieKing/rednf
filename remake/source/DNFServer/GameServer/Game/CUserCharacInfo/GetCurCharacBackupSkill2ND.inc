const char* CUserCharacInfo::GetCurCharacBackupSkill2ND() const
{
    return m_selected ? (const char*)&CUR->m_backupSkill2ND : 0;
}