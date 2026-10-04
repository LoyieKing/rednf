void CUserCharacInfo::setLinkCharacSlotIndex(char index)
{
    if (m_selected)
    {
    m_selected->m_linkCharacSlotIndex = index;
    }
}