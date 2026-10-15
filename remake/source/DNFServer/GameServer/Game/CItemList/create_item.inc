void CItemList::create_item(ENUM_ITEM_CREATE_TYPE type, Inven_Item& item,
                            int param)
{
    item.set_add_info(param);
    CItem* pItem = find_item(item.m_addInfo);
    if (!pItem)
    {
        LogManager::logFormat(1, "item.cpp",
            __PRETTY_FUNCTION__,
            0x98f, "find_item(%d)", item.m_addInfo);
        item.m_addInfo = -1;
        item.set_add_info(0);
        item.m_endurance = 0;
        item.m_itemType = 0;
        item.ResetItemAttr();
        return;
    }
    pItem->make_item(item);
    if (pItem->is_stackable())
    {
        if (item.get_add_info() < 0)
        {
            item.set_add_info(0);
        }
        if (item.get_add_info() > 999)
        {
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x9a1, 5)(
                "CItemList::create_item, add_info >= STACK_LIMIT_ERROR , %d ITEM #%d",
                item.get_add_info(), item.m_addInfo);
        }
    }
    item.m_amp.reset();
    if (type == 1)
    {
        if (GlobalData::s_itemAmplifier_->checkInvestableItem(pItem, item, 0))
        {
            GlobalData::s_itemAmplifier_->generateAmplifyItem(pItem, item);
        }
        if (item.m_itemType == 1)
        {
            int equip = ((CEquipItem*)pItem)->get_endurance();
            int rand = get_rand_int(100);
            int rate = ((CDataManagerExt*)G_CDataManager())->m_amplifyRate;
            int bonus = (int)((float)equip * ((float)(100 - rate) / 100.0f));
            // [bughunt13] ORIG 0x085121ea `flds 0x8c96d98` = 0xc2c80000 = -100.0f（第一处 0x8c96d94 才是
            //   +100.0f）⇒ dur + bonus*(rand/-100) = 随机**扣减**耐久；旧码 /100.0f 恒 ≥ equip 后被夹回满耐久。
            int result = (int)((float)equip + (float)bonus * ((float)rand / -100.0f));
            if (result < 0)
            {
                result = 1;
            }
            else if (result > equip)
            {
                result = equip;
            }
            item.m_endurance = (unsigned short)result;
        }
    }
}