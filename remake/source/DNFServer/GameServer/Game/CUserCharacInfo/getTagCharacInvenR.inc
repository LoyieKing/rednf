CInventory* CUserCharacInfo::getTagCharacInvenR() const
{
    return const_cast<CInventory*>(&m_tag->m_curCharacInven);
}
