void CAvatarItemMgr::AddAvatarItem(int key, Avatar_Item* item)
{
    m_registeredItems.insert(std::make_pair(key, item));
    AddEmblemEndurance(key, item);
