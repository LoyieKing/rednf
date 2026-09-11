void CUserCharacInfo::IncCurCharacEventAddItemCount(unsigned short count)
{
    if (m_selected)
    {
    CUR->m_curCharacEventAddItemCount =
        (unsigned short)(CUR->m_curCharacEventAddItemCount + count);
    }
}