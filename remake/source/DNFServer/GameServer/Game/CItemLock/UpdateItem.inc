void CItemLock::UpdateItem(CUser* user, Inven_Item* item)
{
    if (item->m_field1 == 8)
        UpdateAvatarItem(user, item);
    else if (item->m_field1 == 5)
        UpdateCreatereItem(user, item);
