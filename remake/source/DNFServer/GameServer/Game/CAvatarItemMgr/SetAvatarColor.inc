bool CAvatarItemMgr::SetAvatarColor(int key, short color1, short color2)
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return false;
    item->setColor(color1, color2);
    return true;
}