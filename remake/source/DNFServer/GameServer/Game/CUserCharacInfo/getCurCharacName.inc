const char* CUserCharacInfo::getCurCharacName() const
{
    return m_selected ? (const char*)CUR->m_name : "";
