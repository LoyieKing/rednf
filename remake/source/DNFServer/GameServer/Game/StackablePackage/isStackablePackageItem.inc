bool EventProcessManager::isStackablePackageItem(int itemId) const
{
    CItem* item = G_CDataManager()->find_item(itemId);
    if (item == 0)
        return false;
    if (!item->is_stackable())
        return false;
    CItem* it = item;
    return it->GetItemType() == 0x26;
