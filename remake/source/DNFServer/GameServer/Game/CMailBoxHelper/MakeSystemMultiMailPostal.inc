int CMailBoxHelper::MakeSystemMultiMailPostal(std::vector<std::pair<int, int> >& items,
                                              Inven_Item* itemBase, unsigned int nMax)
{
    // ORIG 0x08556a14
    if (items.size() > 0xa || items.size() == 0)
        return 0;
    unsigned int slot = 0;
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin();
         it != items.end() && slot < nMax; slot++, it++)
    {
        Inven_Item tmp;
        tmp.reset();
        tmp.m_addInfo = it->first;
        G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, tmp,
                                                  it->second);
        itemBase[slot].setCopy(tmp);
    }
    return 1;
}