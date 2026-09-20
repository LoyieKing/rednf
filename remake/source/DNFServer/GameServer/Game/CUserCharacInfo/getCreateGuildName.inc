char* CUserCharacInfo::getCreateGuildName()
{
    return m_selected ? (char*)m_selected->m_padDC5 : 0;
}