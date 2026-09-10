void CharacAdvanceAltarManager::setUnlockBuyItem(AdvanceAltarShopType::T shopType,
                                                 int stageIdx,
                                                 std::vector<_BuyItem>* out)
{
    // [b28d-solo-advancealtar_Manager] ORIG 头两条 `movl $0,-0x1c/-0x18` ⇒ 两段式
    // 指针局部（P197③）；upgrades/items 各自独立 `je` 出口（P133）；循环 it++（ppEi，
    // P28）；entry 赋值序 field4→field0（ORIG -0x58 先写）；make_pair 首参
    // &entry.m_field4（ORIG lea +0x4）。
    std::vector<BuyUpgradeData>* upgrades = 0;
    std::map<int, _BuyItem>* items = 0;
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return;
    }
    upgrades = G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId, shopType);
    items = cur->getBuyItemMap(shopType);
    if (upgrades == 0)
    {
        return;
    }
    if (items == 0)
    {
        return;
    }
    for (std::vector<BuyUpgradeData>::iterator it = upgrades->begin();
         it != upgrades->end(); it++)
    {
        if (it->m_field4 == stageIdx)
        {
            _BuyItem entry;                    // ORIG 循环体首条 _BuyItem::C1
            int minLevel = 0;
            // ORIG +0xd2 `movl $0x1,0x4(%esp)` ⇒ ft=1=FT_INDEX（非 FT_LEVEL=0）：
            // 真语义 bug 修复（原 FT_LEVEL 读错字段列）。
            FieldData* minPoint = it->getMinLevelFieldDataPoint(FieldType::FT_INDEX, minLevel);
            FieldData* pricePoint = it->getFieldDataPoint(minLevel, FieldType::FT_PRICE);
            if (minPoint != 0 && pricePoint != 0)
            {
                entry.m_field4 = it->m_nItemId;
                entry.m_field0 = shopType;
                entry.m_short8 = static_cast<short>(minLevel);
                entry.m_short10 = static_cast<short>(pricePoint->m_value);
                items->insert(std::make_pair(entry.m_field4, entry));
                if (minLevel >= 1 && it->m_field8 != 0)   // ORIG 双条件后才占空槽
                {
                    // ORIG +0x1bf 从 entry.m_field0(-0x5c) 读首参（非形参槽）。
                    setUnlockItemToEmptySlot(
                        static_cast<AdvanceAltarShopType::T>(entry.m_field0),
                        entry.m_field4);
                }
                if (out != 0)
                {
                    out->push_back(entry);
                }
            }
        }
    }
    cur->m_update = 1;
