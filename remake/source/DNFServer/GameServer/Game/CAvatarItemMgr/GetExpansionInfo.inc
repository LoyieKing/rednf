void* CAvatarItemMgr::GetExpansionInfo(int key) const
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return 0;
    return item->getExpansionInfo();
}