void CUserCharacInfo::setDisguiseCharac(bool flag, unsigned char kind,
                                        unsigned short index)
{
    if (m_selected)
    {
    CUR->m_isDisguiseCharac = flag;
    setDisguiseKind(kind);
    setDisguiseIndex(index);
    }
