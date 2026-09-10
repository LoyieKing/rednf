bool _isUnusableSpecialItemInVillage(CItem const* item)
{
    if (!(item->GetItemType() == 0x10 || item->GetItemType() == 0x22))
        return true;
    return false;
