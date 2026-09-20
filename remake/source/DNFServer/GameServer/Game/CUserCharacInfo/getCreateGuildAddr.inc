char* CUserCharacInfo::getCreateGuildAddr()
{
    return m_selected ? (char*)m_selected->m_padDDC : 0;
}