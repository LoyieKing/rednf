// ---- respond_special_item_booster —— ORIG 0x0827ca4a ----
// setOriginalItem → _onBuySpecialItem(user, 6, itemIdx) → 遍历 find_item(itemIdx)
// 的 booster 子物品表 getBoosterItem，逐项 _processAdditionalItem →
// GetRestrictCode/IsRestrictedGoods/SetRestrictedGoods/SaveFeaturedIdx → SaveInventory。
// 子物品项 vtable+0x60 槽经 CItem 虚表序核对 = set_parent_booster_item_id(ulong)，
// 已按真实虚方法直调（CItem.h 声明，槽位 +0x60 吻合）。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82] 205/201；P167 CLEAR 29/29
// （const_iterator 修法消 5 ptEv+2 ne/ppEv 差集），余槽号/寄存器分配。
void SpecialItemHandler::respond_special_item_booster(int,
                                                      const STSpecailItem_Param& param,
                                                      STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    std::vector<Inven_Item>* items = static_cast<std::vector<Inven_Item>*>(param.m_items);
    WongWork::CCeraShop::stCeraShopItemParam_t* cera =
        static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam);
    // ORIG 0x0827ca76 `mov 0x14(%eax),%eax; mov %eax,-0x2c(%ebp)`：count 读入槽后
    // 不再被读（-0x2c 随后被 restrictCode 覆盖）——4.1 保留的死读取（P171）。
    int count = param.m_count;
    (void)count;
    // ORIG 0x0827ca7f：time 具名槽 -0x28（收尾 SaveFeaturedIdx 第 3 实参直读槽）。
    long time = param.m_time;

    m_owner->setOriginalItem(itemIdx);
    m_owner->_onBuySpecialItem(user, ENUM_CERA_SPECIALITEM_BOOSTER,
                               static_cast<unsigned long>(itemIdx));

    // ORIG 0x0827cabf `movl $0x0,-0x24(%ebp)`：写后不读的死局部（P171 照抄）。
    int deadSlot = 0;
    (void)deadSlot;

    CStackableItem* base = static_cast<CStackableItem*>(G_CDataManager()->find_item(itemIdx));
    std::vector<stStackableBoosterItemInfo_t> boosterList;
    // ORIG 0x0827caf5：find_item 结果直调 getBoosterItem（无判空 cmpl/je）。
    base->getBoosterItem(boosterList);

    // b28d-solo(P167 判据)：ORIG 0x0827ca4a 用 const_iterator（ne/ppEv/operator-> 全
    // const 版 + iterator→const_iterator 转换构造 ×2）；非 const 版多发/少发上述符号。
    for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it = boosterList.begin(),
             itEnd = boosterList.end();
         it != itEnd; ++it)
    {
        // 实测：帧仍 0x8c（未跨档）、报告 diff 124/124→117/119、our 205→203 ⇒
        // 保留（不劣化且收敛）；余 1 槽待后续定位。
        // [G10 实验·P231/P52] ORIG 浅区槽比本形少 1（我方多 0x40）⇒ subItem 跨
        // set_parent_booster_item_id 虚调保活，试纯 register（P0 合规、非 asm 绑定）。
        register CItem* subItem = G_CDataManager()->find_item((int)it->m_itemIdx);
        // ORIG vtable+0x60 = CStackableItem::set_parent_booster_item_id(unsigned long)
        if (subItem != 0)
            subItem->set_parent_booster_item_id((unsigned long)itemIdx);
        // _processAdditionalItem 实参：itemIdx, addInfo(+0x8), avatarLevel(+0xe), avatarType(+0xd)
        m_owner->_processAdditionalItem(user, (unsigned long)it->m_itemIdx, it->m_addInfo,
                                        it->m_avatarLevel, it->m_avatarType, *items, *cera, 0);
        if (subItem != 0)
            subItem->set_parent_booster_item_id(0);
    }

    int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
    if (restrictCode != 0)
    {
        if (!user->IsRestrictedGoods(restrictCode))
        {
            user->SetRestrictedGoods(restrictCode);
            m_owner->SaveFeaturedIdx(user, restrictCode, (int)time, 0);
        }
    }
    user->SaveInventory();
}

