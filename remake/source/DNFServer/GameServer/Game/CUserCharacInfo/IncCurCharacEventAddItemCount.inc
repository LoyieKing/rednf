void CUserCharacInfo::IncCurCharacEventAddItemCount(unsigned short count)
{
    if (m_selected)
    {
    m_selected->m_eventAddItemCount =
        (unsigned short)(m_selected->m_eventAddItemCount + count);
    }
}