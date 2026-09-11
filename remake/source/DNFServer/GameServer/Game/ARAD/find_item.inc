bool Arad_OnePlusCardEvent_ItemList::find_item(unsigned short webId, Data& out)
{
    // ORIG 0x081a398e：Data(webId,0) + std::find → operator==（0x081a420a）按 webId 匹配。
    Data tmp(webId, 0);
    std::vector<Data>::iterator it = std::find(m_cardList.begin(), m_cardList.end(), tmp);
    if (it == m_cardList.end())
        return false;
    out.webId = it->webId;
    out.itemIndex = it->itemIndex;
    return true;
}