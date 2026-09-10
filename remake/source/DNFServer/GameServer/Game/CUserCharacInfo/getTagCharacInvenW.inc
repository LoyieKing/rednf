Inven_Item* CUserCharacInfo::getTagCharacInvenW()
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (Inven_Item*)((CharacInfoFields*)m_tag)->m_inven;
