void CUserCharacInfo::IncCurCharacEventAddItemCount(unsigned short count)
{
    if (m_selected)
    {
    m_selected->m_field123C =
        (unsigned short)(m_selected->m_field123C + count);
    }
}