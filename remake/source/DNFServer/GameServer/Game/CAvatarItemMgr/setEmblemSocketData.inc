int CAvatarItemMgr::setEmblemSocketData(int key, const stAvatarEmblemInfo_t& emblem)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    item->setEmblemSocketData(emblem);
    AddEmblemEndurance(key, item);
    return 1;
}