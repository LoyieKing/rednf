void CUserCharacInfo::setDisguiseKind(unsigned char kind)
{
    if (m_selected)
    {
    CUR->m_disguiseKind = kind;
    }
