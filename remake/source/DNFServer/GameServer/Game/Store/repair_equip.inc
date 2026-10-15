// ---------------------------------------------------------------------------
// repair_equip(CUser*, char invenType, short slot, ushort count, ushort& out) ——
// ORIG 0x08619cec（docs/class_func_reports/Store/repair_equip.md 汇编+反编译 C）。
// slot==-1：全修 —— 装备栏 3..8（invenType 1）+ 背包 10..0x15（invenType 0）扫描，
//   收集需修装备（maxEnd!=0 且 curEnd!=maxEnd，objdump +0xc00/+0xe90），累计价格后
//   use_money(totalCost, reason 1, log 1)；成功先统计(0x14)+死亡之塔(仅 state==10、
//   dungeon idx!=11000) 上报，再逐件重取槽写满耐久 update_item，最后对背包
//   10..0x15 发 SendUpdateItem(1, space 3)；失败返 10。返回 0x13=交易中。
// 单件：msg invenType 0→装备栏(type 1)/2→cargo/3→背包(type 0)，其余返 0x11；
//   空槽或非 ITEM_TYPE_EQUIP 返 0x11；*out=当前耐久；查表空/满耐久返 0x11；
//   count!=0xffff 时从 GetInvenSlot(1,count) 取工具（须 ITEM_TYPE_2 且
//   m_addInfo==0x1f77/0x201f，否则 0x15）→ use_item(count,1,0) 成功扣 1 并把
//   修理费置 0（失败 0x16）；use_money 失败返 10；耐久写满后按 msg 空间
//   update_item + SendUpdateItem(1, space 0/2/3)。
// 字段口径：判空/查表/工具 id 全程直读 +0x02 m_addInfo（item idx）；耐久为
//   +0x0b m_endurance。m_addInfo2(+0x07, get_add_info) 是 stackable 数量域，勿混。
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 判词：DIFF·N82/N85 封顶（语义完成）。
// 本轮修正（全部有 ORIG 实证）：① find_item 目标互换——第一循环（装备栏 3..8）是
// CItemList::find_item、第二循环（背包 10..0x15）是 CDataManager::find_item，旧实现写反；
// ② get_charac_level/get_endurance 逐次重读（各 +3/+4 调用点）；③ 循环内结果复用函数头
// item（4.4 对 `Inven_Item x=f()` 直用 sret 临时，N86；改赋值后逐字段拷贝形与 ORIG 一致，
// sret 临时槽 7 调用点复用同址）；④ 两处分派与 update_item 分派改 switch（ORIG 无槽
// movsbl+cmp 决策树，P251/P286）；⑤ if/else 块序交换（ORIG je 落 slot==-1 块，P59/P222）；
// ⑥ get_state/use_money/is_stackable 表达式直用（P0 无槽）；⑦ notTower 改 P158 形。
// 残余（p167 CLEAR 114=114 硬证据下）：帧 0x22c vs 0x1bc + 全帧槽号平移（N82/N95），
// rate 实参 4.1 flds/fstps 栈往返 vs 4.4 mov（N85），迭代器 operator* 返回值两段拷贝
// 与寄存器角色互换（N7/N18a）。gap = -24/1067（2.2%）。
// [P2S6 2026-09-11 本波] 逐件实测（改前 n1_v 1075/1091 gap=-16、fdiff blocks=204/
//   不等长 44/net O−N=−12）→ 改后 fdiff **blocks=175、ours=1067 = orig=1067（净 0）**、
//   n1_v 1075/1067 gap=+8（差值为 ORIG 尾部 nop/对齐）。四数 25/50/0/2 全程不变。
//   落地旋钮（全部「ORIG 有没有那个槽」判据）：
//   ① tower 链去具名（两处）：ORIG 0x0861a3f8+0x0711 / 0x0861ac7f+0x0f93 只 tower 落槽
//      （-0x40 / -0x20），getCDungeonMGr(0x08234612)/getDungeon(0x082345f0)/
//      get_index(0x080fdcf0) 三次调用全内联；notTower 形 = `cmp $0x2af8,%eax; sete %al;
//      test %al,%al; je L; movb $0x0,-0x41(%ebp); jmp; L: movb $0x1,-0x41(%ebp)`
//      ⇒ 源码写三元 `(idx==11000) ? false : true`（我方 4.4.7 生成 setne+直存，属版本差）。
//   ② hasAmp/upgrade 去具名（三处）：ORIG 0x0861a282+0x0596 `call Inven_Item::GetUpgrade;
//      movzbl %al,%ebx`（直留 %ebx）+ hasAmp 留 %esi ⇒ 两实参内联进
//      getUpgradeItemRepairCostRate(item.GetUpgrade(), item.m_amp.hasAbility())。
//   ③ maxEnd 去具名（三处）：ORIG 0x0861a2ba 无 maxEnd 槽，耐久在实参处现取。
//   负结论：把 `int cost` 内联成 `totalCost += getEquipmentRepairPrice(...)`（ORIG
//   0x0861a8xx 形为 `call; add %eax,-0x58(%ebp)`）实测**劣化**：不等长块 31→57、
//   net O−N 0→+4 ⇒ 已回滚（保留具名 cost）。
//   剩余差异性质：全帧槽号平移（我方 Inven_Item 基址 -0x1a8 vs ORIG -0x168，帧 0x20c
//   vs 0x1bc）；rate 的 4.1 `flds/fstps` vs 4.4 `mov`（N85）；bool 物化 `sete+分支`
//   vs 4.4 `setne`（notTower/needRepair）；`ok` 的 %esi 保活 vs 我方槽 —— 皆 4.1/4.4
//   前/后端行为差，源码层不可造。
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
        if (item.m_addInfo == 0 || item.m_itemType != Inven_Item::ITEM_TYPE_EQUIP)
            return 0x11;

        outEndurance = item.m_endurance;

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
            if (item2.m_addInfo == 0 || item2.m_itemType != Inven_Item::ITEM_TYPE_2)
                return 0x15;
            // ORIG 0x0861bxxx：直接读 item2.m_addInfo 两次（P48，无具名 toolIdx）。
            if (item2.m_addInfo != 0x1f77 && item2.m_addInfo != 0x201f)
                return 0x15;
        }

        // ORIG 0x0861bxxx：两次独立 get_charac_level()；价格出参复用 totalCost
        // 同一栈槽（ORIG local_5c，非新局部）。
        int growthGrade = pEquip->getGrowthGrade(user->get_charac_level());
        int growthCost = pEquip->getGrowthRepairCost(user->get_charac_level());
        // [P2S6 2026-09-11 N82 去槽] ORIG 0x0861a282 +0x0596..+0x05a7：
        //   `call Inven_Item::GetUpgrade; movzbl %al,%ebx`（结果直留 %ebx，**无 upgrade
        //   槽**）；hasAmp 更早求值留 %esi（**无 hasAmp 槽**）；再 `call G_CDataManager`
        //   ⇒ 右→左求值序 (arg3 hasAmp, arg2 upgrade, arg1 manager) ⇒ 两实参均内联。
        float rate = G_CDataManager()->getUpgradeItemRepairCostRate(
            item.GetUpgrade(), item.m_amp.hasAbility());
        // [P2S6 2026-09-11 N82 去槽] ORIG 0x0861a2ba 附近无 maxEnd 槽：耐久在实参处
        //   现取（`movzwl` 序列），源码为内联调用。
        totalCost = getEquipmentRepairPrice(growthCost, item.m_endurance, pEquip->get_endurance(),
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
                // [P2S6 2026-09-11 N82 去多余具名槽] ORIG 0x0861a3f8 +0x0711..+0x0747：
                //   仅 tower 落槽 -0x40；getCDungeonMGr(0x08234612)/getDungeon(0x082345f0)/
                //   get_index(0x080fdcf0) 三次调用**全内联**（无 mgr/dun/idx 槽，我方原
                //   各留 4 字节槽 ⇒ 帧 +0x60、每处多 mov 往返）。
                //   notTower(-0x41) 形 = `cmp $0x2af8,%eax; sete %al; test %al,%al; je L;
                //   movb $0x0,-0x41(%ebp); jmp; L: movb $0x1,-0x41(%ebp)` ⇒ 源码是三元
                //   `(idx==11000) ? false : true`（**非** if(缺 else) 形）。
                bool notTower =
                    tower->getCDungeonMGr()->getDungeon()->get_index() == 11000 ? false : true;
                int lvl = user->get_charac_level();
                GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                    (DEATHTOWER_VALUE_STATISTIC_FIELD)9, (char)notTower, (char)lvl, totalCost);
            }
        }

        item.m_endurance = (unsigned short)pEquip->get_endurance();

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
            if (item.m_addInfo != 0 && item.m_itemType == Inven_Item::ITEM_TYPE_EQUIP)
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
                    // [P2S6 2026-09-11 N82 去槽] 同单件分支：ORIG +0x0bb6 前
                    //   `movzbl %al,%ebx`（GetUpgrade 直留寄存器）+ hasAmp 留 %esi。
                    float rate = G_CDataManager()->getUpgradeItemRepairCostRate(
                        item.GetUpgrade(), item.m_amp.hasAbility());
                    // [P2S6 2026-09-11 负结论] 试过把 `int cost` 内联成
                    //   `totalCost += getEquipmentRepairPrice(...)`（ORIG 0x0861a8xx 是
                    //   `call; add %eax,-0x58(%ebp)`）⇒ 实测**劣化**：blocks 175→174 但
                    //   不等长块 31→57、net O−N 0→+4（我方少 4 条）；4.4 对 `+=` 形会
                    //   额外折叠。保留具名 cost（与 fdiff net=0 的最优态一致）。
                    int cost = getEquipmentRepairPrice(growthCost, item.m_endurance, pEquip->get_endurance(),
                                                        growthGrade, false, rate);
                    totalCost += cost;

                    // ORIG 0x0861a8xx：needRepair 两次独立 get_endurance() 重调
                    // （maxEnd==0 → false；否则 m_endurance==get_endurance() → false）。
                    bool needRepair = true;
                    if (pEquip->get_endurance() == 0)
                        needRepair = false;
                    else if (item.m_endurance == pEquip->get_endurance())
                        needRepair = false;
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(INVEN_TYPE_EQUIP, i, pEquip));
                }
            }
        }

        for (int i = 10; i < 0x16; i++)
        {
            item = user->getCurCharacInvenR()->GetInvenSlot(0, i);
            if (item.m_addInfo != 0 && item.m_itemType == Inven_Item::ITEM_TYPE_EQUIP)
            {
                // ORIG 0x0861abxx：第二循环（背包 10..0x15）走 CDataManager::find_item
                // （0x0835fa32，非 m_itemList）——与第一循环相反。
                CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
                if (pEquip)
                {
                    int growthGrade = pEquip->getGrowthGrade(user->get_charac_level());
                    int growthCost = pEquip->getGrowthRepairCost(user->get_charac_level());
                    // [P2S6 2026-09-11 N82 去槽] 同单件分支（ORIG +0x0e40 前同形）。
                    float rate = G_CDataManager()->getUpgradeItemRepairCostRate(
                        item.GetUpgrade(), item.m_amp.hasAbility());
                    int cost = getEquipmentRepairPrice(growthCost, item.m_endurance, pEquip->get_endurance(),
                                                        growthGrade, false, rate);
                    totalCost += cost;   // 见第一循环注：`+=` 内联形被实测否证（net +4）。

                    bool needRepair = true;
                    if (pEquip->get_endurance() == 0)
                        needRepair = false;
                    else if (item.m_endurance == pEquip->get_endurance())
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
                    // [P2S6 2026-09-11 N82] 同单件分支：ORIG 0x0861ac7f +0x0f93..+0x0fd2
                    //   仅 tower 落槽 -0x20，三次调用全内联 + 三元 notTower。
                    bool notTower =
                        tower->getCDungeonMGr()->getDungeon()->get_index() == 11000 ? false : true;
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
                if (item.m_addInfo != 0 && item.m_itemType == Inven_Item::ITEM_TYPE_EQUIP)
                {
                    // ORIG 0x0861b0xx：直接 store get_endurance()（无具名 maxEnd 槽，P0）。
                    item.m_endurance = (unsigned short)pEquip->get_endurance();
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
}