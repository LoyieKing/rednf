void CUserCharacInfo::setDisguiseKind(unsigned char kind)
{
    if (m_selected)
    {
    m_selected->m_disguiseKind = kind;
    }
}