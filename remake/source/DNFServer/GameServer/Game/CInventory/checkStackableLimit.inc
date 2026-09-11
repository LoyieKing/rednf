bool checkStackableLimit(unsigned long itemIdx, int count)
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item != 0 && item->is_stackable())
    {
        CStackableItem* sitem = static_cast<CStackableItem*>(item);
        if (sitem->getStackableLimit() < count || count < 0)
        {
            return false;
        }
    }
    return true;
}