int DisPatcher_BuyItem::get_data(CUser* user, MSG_BUY_ITEM& msg, BuyItemParam& param)
{
    CDataManager* dataMgr = G_CDataManager();
    param.m_a8 = dataMgr->find_item(msg.m_itemIdx);
    if (param.m_a8 == 0)
    {
        return 0x11;
    }
    // GetNeedMaterial() 返回 +0x24 处 pair<int,int>*（item idx, 数量），ORIG 读两 int
    const int* needMaterial =
        reinterpret_cast<const int*>(param.m_a8->GetNeedMaterial());
    param.m_pair.first = needMaterial[0];
    param.m_pair.second = needMaterial[1];
    if (param.m_pair.first != -1)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        param.m_a4 = invenR->GetInvenData(param.m_pair.first, param.m_resultItem);
        CDataManager* dataMgr2 = G_CDataManager();
        param.m_ac = dataMgr2->find_item(param.m_pair.first);
        if (param.m_ac == 0 || param.m_ac->is_stackable() != true)
        {
            return 1;
        }
    }
    param.m_80 = param.m_a8->get_price();             // ORIG 0x0822c84a W
    param.m_84 = param.m_a8->GetCashPrice();          // ORIG 0x0822c856 W
    param.m_88 = param.m_a8->GetMedalPrice();
    param.m_8c = param.m_a8->GetFinishPointPrice();
    param.m_90 = param.m_a8->GetGuildPowerWarPoint();   // ORIG vtable slot +0x28
    param.m_94 = param.m_a8->GetUserPowerWarPoint();    // ORIG vtable slot +0x2c
    if (param.m_a8->is_stackable() != true)
    {   // 非堆叠：msg+0x11 须等于当日种子（防复制）
        if (msg.m_buyCount != user->GetSeedFromDate())
        {
            return 1;
        }
    }
    else
    {
        if (msg.m_buyCount == 0)
        {
            return 1;
        }
        if ((unsigned int)msg.m_buyCount > 1)
        {
            if (get_nMaxBuyableCount(user, param) < (unsigned int)msg.m_buyCount)
            {
                return 10;
            }
            param.m_80 *= msg.m_buyCount;
            param.m_84 *= msg.m_buyCount;
            param.m_88 *= msg.m_buyCount;
            param.m_8c *= msg.m_buyCount;
            param.m_90 *= msg.m_buyCount;
            param.m_94 *= msg.m_buyCount;
            param.m_pair.second = msg.m_buyCount * param.m_pair.second;
        }
    }
    // ---- 余额校验：任一货币不足 → 10 ----
    bool insufficient =
        ((unsigned int)user->getCurCharacInvenR()->get_money() < (unsigned int)param.m_80) ||
        ((unsigned int)user->GetCera() < (unsigned int)param.m_84) ||
        ((unsigned int)user->GetWinPoint() < (unsigned int)param.m_88) ||
        ((unsigned int)user->GetFinishPoint() < (unsigned int)param.m_8c) ||
        ((param.m_pair.first != -1 &&
          param.m_resultItem.m_addInfo2 < param.m_pair.second)) ||
        ((unsigned int)user->GetGuildPowerWarPoint() < (unsigned int)param.m_90) ||
        ((short)user->GetUserPowerWarPoint() < (short)param.m_94);
    if (insufficient)
    {
        return 10;
    }
    // 公会点数商品仅会长可购（vtable slot +0x28 复读）
    if (param.m_a8->GetGuildPowerWarPoint() != 0 && !user->IsGuildMaster())
    {
        return 0x24;
    }
    return 0;
