int CClearReward::generatePcRoomCardBlankItem(Inven_Item& item)
{
    if (m_pcRoomBlanks.empty())
        return 0;
    int cum = 0;
    int pick = get_rand_int(m_pcRoomBlankTotal);
    // [b28d-63] 同 generateGoldCardBlankItem：ORIG 0x0853904f 起 const_iterator 遍历。
    std::vector<stGoldCardBlankItemInfo_t>::const_iterator it = m_pcRoomBlanks.begin();
    std::vector<stGoldCardBlankItemInfo_t>::const_iterator endIt = m_pcRoomBlanks.end();
    while (it != endIt)
    {
        cum += it->v[0];
        if (pick < cum)
        {
            if (it->v[1] == -1)
                return 0;
            item.m_addInfo = it->v[1];
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), item, it->v[2]);
            return 1;
        }
        ++it;
    }
    return 0;
