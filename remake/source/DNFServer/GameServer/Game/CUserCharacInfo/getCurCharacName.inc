const char* CUserCharacInfo::getCurCharacName() const
{
    return m_selected ? (const char*)m_selected->m_name : "";
}