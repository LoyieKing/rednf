CInventory* CUserCharacInfo::getTagCharacInvenR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (CInventory*)((CharacInfoFields*)m_tag)->m_inven;
}