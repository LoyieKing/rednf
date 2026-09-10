const char* CAvatarItemMgr::GetIPGAgencyNo(int addInfo) const
{
    Avatar_Item* item = _FindItemAll(addInfo);
    if (item == 0)
        return 0;
    return item->GetIPGAgencyNo();
