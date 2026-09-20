const char* CUserCharacInfo::GetCurCharacBackupSkill() const
{
    return m_selected ? (const char*)&m_selected->m_premiumLethe : 0;
}