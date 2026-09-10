int CAvatarItemMgr::GetRemainDate(int nAvatarUid, int nTick) const
{
    Avatar_Item* item = _FindItemAll(nAvatarUid);
    if (item == 0)
        return -1;
    return item->GetRemainDate(nTick);
