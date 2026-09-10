bool IsTradeLimitAttachTypeItem(Inven_Item const& item)
{
    CItem* found = G_CDataManager()->find_item(item.m_addInfo);
    if (found == 0)
        return false;
    if (!found->is_stackable())
        return false;
    CItem* it = found;
    return it->GetAttachType() == 7;
