int CAvatarItemMgr::GetExpireDate(int avatarUid) const
{
    Avatar_Item* item = _FindItemAll(avatarUid);
    if (item == 0)
        return -1;
    return item->GetExpireDate();
}