// ---- ORIG T 0x081319ae（buyItem.md 全文）----
// [b28d-solo-advancealtar_Manager AE] 本次修：正极性 `if (it==end){插入}else{更新}`
// （P220）+ make_pair(level, entry) 首参取形参地址 + 更新分支 `m_short10 +=`（单次
// operator-> 读写同址，P38）⇒ 125=125 逐条全等。
int CharacAdvanceAltarManager::buyItem(AdvanceAltarShopType::T shopType, int level,
                                       short fromSlot, short toSlot, int totalCost,
                                       int& outReward)
{
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return 3;
    }
    if (cur->m_starPoint < totalCost)
    {
        return 2;
    }
    std::map<int, _BuyItem>* items = cur->getBuyItemMap(shopType);
    if (items == 0)
    {
        return 3;
    }

    // [b28d-solo-advancealtar_Manager] ORIG +0xa8 调 eq、`je +0x123`（==end 为假跳
    // 更新块）⇒ 正极性 `if (it == end) {插入} else {更新}`（P220）；make_pair 首参
    // 取形参 level 地址（ORIG `lea 0x10(%ebp)`），非 entry.m_field4。
    std::map<int, _BuyItem>::iterator it = items->find(level);
    if (it == items->end())
    {
        _BuyItem entry;                  // ORIG _BuyItem ctor W memset 12
        entry.m_field0 = shopType;       // 序按 asm：param_2→f0 / param_3→f4 /
        entry.m_field4 = level;          //   toSlot→short8 / totalCost→shortA
        entry.m_short8 = toSlot;
        entry.m_short10 = static_cast<short>(totalCost);
        items->insert(std::make_pair(level, entry));
    }
    else
    {
        it->second.m_short8 = toSlot;                          // ORIG +0xc ← param_5 截断
        it->second.m_short10 += static_cast<short>(totalCost);  // +0xe 累加（ORIG 单次
                                                                // operator-> 读写同址）
    }

    cur->m_starPoint -= totalCost;                              // ORIG +0x14 扣减
    cur->m_update = 1;
    outReward = cur->m_starPoint;
    HistoryLog::buyItem(shopType, level, fromSlot, toSlot, totalCost);
    return 0;
}