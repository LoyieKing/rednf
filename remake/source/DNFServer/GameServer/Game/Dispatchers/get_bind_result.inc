int Dispatcher_MonstercardBind::get_bind_result(
    CUser* user, short slot, ENUM_RARITY rarity,
    std::vector<std::pair<int, int> >& results)
{
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    Inven_Item inven = invenR->GetInvenSlot(1, slot);
    CItem* card = G_CDataManager()->find_item(inven.m_addInfo);
    // +0x330：怪物卡绑定数据索引（CStackableItem::m_int330，ORIG mov 0x330(%eax)）。
    const int cardDataIdx =
        reinterpret_cast<const CStackableItem*>(card)->m_int330;
    STExpertJobScript* script = static_cast<STExpertJobScript*>(
        G_CDataManager()->GetExpertJobScript(1));
    stMonsterCardBindRate rate;
    if (!script->GetMonsterCardBindRate(cardDataIdx, rate))
    {
        return 0;
    }
    results.clear();
    const int newRarity = calcurate_rarity(rarity, rate.m_field0);
    unsigned long range = 10000;
    const unsigned int roll = m_rand->randInt(range);
    if (roll < rate.m_field2)
    {
        return _bind_process(BIND_TYPE_SPECIAL, (ENUM_RARITY)newRarity,
                             results);
    }
    return _bind_process(BIND_TYPE_COMMON, (ENUM_RARITY)newRarity, results);
