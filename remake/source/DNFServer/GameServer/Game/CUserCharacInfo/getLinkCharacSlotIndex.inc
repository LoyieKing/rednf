char CUserCharacInfo::getLinkCharacSlotIndex()
{
    return m_selected ? (int)(unsigned char)m_selected->m_linkCharacSlotIndex : -1;
}