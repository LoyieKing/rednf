int Store::repair_equip(CUser* user, char invenType, short slot, unsigned short count,
                        unsigned short& outEndurance)
{
    if (user->CheckInTrade())
        return 0x13;

    Inven_Item item;
    int totalCost = 0;

    // ORIG 0x08619d0x：`cmpw $0xffff; je <slot==-1 块>` ⇒ 单件路 fall-through 在前、
    // 全修路在 je 目标（后）⇒ 源码条件取反 + 块序交换（P59/P222 布局判据）。
    if (slot != -1)
    {
        // ORIG 0x08619d25：`movsbl invenType,%eax; cmp $2; je; cmp $3; je;
        // test %eax; jne <0x11>` 无槽线性决策树 ⇒ switch(invenType)（P251/P286）。
        switch (invenType)
        {
        case 0:
            item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
            break;
        case 2:
            item = user->getCurCharacCargoR()->get_cargo_slot(slot);
            break;
        case 3:
            item = user->getCurCharacInvenR()->GetInvenSlot(0, slot);
            break;
        default:
            return 0x11;
        }

        // ORIG 全程直读 item+0x02 的 m_addInfo（item idx）；get_add_info() 对应
        // +0x07 m_addInfo2（stackable 数量域），普通装备为 0，不得用于判空/查表。
        if (item.m_addInfo == 0 || item.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
            return 0x11;

        outEndurance = item.m_fieldb;

        // ORIG 0x08619xxx：两个具名指针局部（local_58 = find_item 结果，
        // local_54 = local_58 的 CEquipItem 视图）——P0 槽往返实证。
        CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
        if (pItem == 0)
            return 0x11;
        CEquipItem* pEquip = (CEquipItem*)pItem;

        if (pEquip->get_endurance() == 0)
            return 0x11;

        Inven_Item item2;
        if (count != 0xffff)
        {
            item2 = user->getCurCharacInvenR()->GetInvenSlot(1, count);   // ORIG GetInvenSlot(1, count)
            if (item2.m_addInfo == 0 || item2.m_field1 != Inven_Item::ITEM_TYPE_2)
                return 0x15;
            // ORIG 0x0861bxxx：直接读 item2.m_addInfo 两次（P48，无具名 toolIdx）。
            if (item2.m_addInfo != 0x1f77 && item2.m_addInfo != 0x201f)
                return 0x15;
        }

        // ORIG 0x0861bxxx：两次独立 get_charac_level()；价格出参复用 totalCost
        // 同一栈槽（ORIG local_5c，非新局部）。
        int growthGrade = pEquip->getGrowthGrade(user->get_charac_level());
        int growthCost = pEquip->getGrowthRepairCost(user->get_charac_level());
        bool hasAmp = item.m_amp.hasAbility();
        unsigned char upgrade = item.GetUpgrade();
        float rate = G_CDataManager()->getUpgradeItemRepairCostRate(upgrade, hasAmp);
        int maxEnd = pEquip->get_endurance();
        totalCost = getEquipmentRepairPrice(growthCost, item.m_fieldb, maxEnd,
                                             growthGrade, false, rate);

        if (count != 0xffff && (item2.m_addInfo == 0x1f77 || item2.m_addInfo == 0x201f))
        {
            CInventory* invenW = user->getCurCharacInvenW();
            int ret = invenW->use_item(count, 1, 0);
            if (ret != 0)
                return 0x16;
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, count);
            totalCost = 0;
        }

        // ORIG 0x08619xxx：`call use_money; xor $1; test %al` 无槽 ⇒ 表达式直用（P0）。
        if (!user->getCurCharacInvenW()->use_money(totalCost, (eMoneySubReason)1, true))
            return 10;

        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x14, user, totalCost);

        // ORIG 0x08619xxx：`call get_state; cmp $0xa,%eax; sete` 无槽往返 ⇒ 表达式直用（P0）。
        if (user->get_state() == 10)
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower)
            {
                WongWork::CDeathTower::CDungeonMgr* mgr = tower->getCDungeonMGr();
                CDungeon* dun = mgr->getDungeon();
                int idx = dun->get_index();
                // ORIG 0x08619xxx：`cmp $0x2af8; sete; test; je; movb $0; jmp; movb $1`
                // ⇒ bool 初值 true + 条件置 false（P158）。
                bool notTower = true;
                if (idx == 11000)
                    notTower = false;
                int lvl = user->get_charac_level();
                GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                    (DEATHTOWER_VALUE_STATISTIC_FIELD)9, (char)notTower, (char)lvl, totalCost);
            }
        }

        item.m_fieldb = (unsigned short)pEquip->get_endurance();

        // ORIG 0x08619xxx：`movsbl invenType; cmp $2; je; cmp $3; je` 决策链 ⇒ switch。
        switch (invenType)
        {
        case 0:
            user->getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
            break;
        case 2:
            user->getCurCharacCargoW()->update_item(slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)2, slot);
            break;
        case 3:
            user->getCurCharacInvenW()->update_item((INVEN_TYPE)0, slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, slot);
            break;
        default:
            break;
        }
    }
    else
    {
        std::vector<tagRepairItemInfo> repairList;

        for (int i = 3; i < 9; i++)
        {
            // ORIG 0x0861a719：sret 临时 → 拷贝赋值给**函数头部的 item**（local_d9），
            // 非新声明局部（4.4 对 `Inven_Item x = f()` 会直用临时，N86）。
            item = user->getCurCharacInvenR()->GetInvenSlot(1, i);
            if (item.m_addInfo != 0 && item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                // ORIG 0x0861a73x：第一循环（装备栏 3..8）走 CItemList::find_item
                // （`G_CDataManager()+0xc` 的 m_itemList 成员）。
                CEquipItem* pEquip =
                    (CEquipItem*)G_CDataManager()->m_itemList->find_item(item.m_addInfo);
                if (pEquip)
                {
                    // ORIG 每处 get_charac_level() 独立重调（P157 逐次重读）。
                    int growthGrade = pEquip->getGrowthGrade(user->get_charac_level());
                    int growthCost = pEquip->getGrowthRepairCost(user->get_charac_level());
                    bool hasAmp = item.m_amp.hasAbility();
                    unsigned char upgrade = item.GetUpgrade();
                    float rate = G_CDataManager()->getUpgradeItemRepairCostRate(upgrade, hasAmp);
                    int maxEnd = pEquip->get_endurance();
                    int cost = getEquipmentRepairPrice(growthCost, item.m_fieldb, maxEnd,
                                                        growthGrade, false, rate);
                    totalCost += cost;

                    // ORIG 0x0861a8xx：needRepair 两次独立 get_endurance() 重调
                    // （maxEnd==0 → false；否则 m_fieldb==get_endurance() → false）。
                    bool needRepair = true;
                    if (pEquip->get_endurance() == 0)
                        needRepair = false;
                    else if (item.m_fieldb == pEquip->get_endurance())
                        needRepair = false;
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(INVEN_TYPE_EQUIP, i, pEquip));
                }
            }
        }

        for (int i = 10; i < 0x16; i++)
        {
            item = user->getCurCharacInvenR()->GetInvenSlot(0, i);
            if (item.m_addInfo != 0 && item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                // ORIG 0x0861abxx：第二循环（背包 10..0x15）走 CDataManager::find_item
                // （0x0835fa32，非 m_itemList）——与第一循环相反。
                CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
                if (pEquip)
                {
                    int growthGrade = pEquip->getGrowthGrade(user->get_charac_level());
                    int growthCost = pEquip->getGrowthRepairCost(user->get_charac_level());
                    bool hasAmp = item.m_amp.hasAbility();
                    unsigned char upgrade = item.GetUpgrade();
                    float rate = G_CDataManager()->getUpgradeItemRepairCostRate(upgrade, hasAmp);
                    int maxEnd = pEquip->get_endurance();
                    int cost = getEquipmentRepairPrice(growthCost, item.m_fieldb, maxEnd,
                                                        growthGrade, false, rate);
                    totalCost += cost;

                    bool needRepair = true;
                    if (pEquip->get_endurance() == 0)
                        needRepair = false;
                    else if (item.m_fieldb == pEquip->get_endurance())
                        needRepair = false;
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(INVEN_TYPE_INVEN, i, pEquip));
                }
            }
        }

        CInventory* invenW = user->getCurCharacInvenW();
        bool ok = invenW->use_money(totalCost, (eMoneySubReason)1, true);
        if (ok)
        {
            GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x14, user, totalCost);

            if (user->get_state() == 10)
            {
                WongWork::CDeathTower* tower = user->getDeathTower();
                if (tower)
                {
                    WongWork::CDeathTower::CDungeonMgr* mgr = tower->getCDungeonMGr();
                    CDungeon* dun = mgr->getDungeon();
                    int idx = dun->get_index();
                    bool notTower = true;
                    if (idx == 11000)
                        notTower = false;
                    int lvl = user->get_charac_level();
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)9, (char)notTower, (char)lvl, totalCost);
                }
            }


            // ORIG 0x0861afxx：begin()/end() 各自经 const_iterator 转换 ctor 落具名槽
            // （P28/P270；直接写 repairList.end() 会选 (const,non-const) 混合 operator!=）。
            std::vector<tagRepairItemInfo>::const_iterator it = repairList.begin();
            std::vector<tagRepairItemInfo>::const_iterator itEnd = repairList.end();
            for (; it != itEnd; ++it)
            {
                const tagRepairItemInfo& info = *it;
                int invenType2 = info.m_type;
                int slotIdx = info.m_slot;
                CEquipItem* pEquip = const_cast<CEquipItem*>(info.m_item);

                // ORIG 迭代内重取：GetInvenSlot(type, slot) 值版；update 走 W。
                item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)invenType2, slotIdx);
                if (item.m_addInfo != 0 && item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                {
                    // ORIG 0x0861b0xx：直接 store get_endurance()（无具名 maxEnd 槽，P0）。
                    item.m_fieldb = (unsigned short)pEquip->get_endurance();
                    user->getCurCharacInvenW()->update_item((INVEN_TYPE)invenType2, slotIdx, item);
                }
            }

            for (int i = 10; i < 0x16; i++)
                user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, i);

            return 0;
        }
        else
        {
            return 10;
        }
    }

    return 0;
