unsigned char CUserCharacInfo::getDisguiseKind()
{
    return m_selected ? m_selected->m_fieldEEA : 0;
}