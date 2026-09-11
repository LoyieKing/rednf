int CAvatarItemMgr::updateEmblemSocket(int key, unsigned int count, char* grades, unsigned long* itemIds)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    for (unsigned int i = 0; i < count; i++)
    {
        item->updateEmblemSocket((unsigned int)grades[i], (unsigned int)itemIds[i]);
    }
    AddEmblemEndurance(key, item);
    return 1;
}