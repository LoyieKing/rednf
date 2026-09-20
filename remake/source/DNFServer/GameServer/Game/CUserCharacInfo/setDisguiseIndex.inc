void CUserCharacInfo::setDisguiseIndex(unsigned short index)
{
    if (m_selected)
    {
    m_selected->m_fieldEEB = index;
    }
}