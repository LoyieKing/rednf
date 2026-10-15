// ===========================================================================
// 7) CUser::multibox_lottery_item(int boxSlot, int keySlot) @0x08674968
// ===========================================================================
// [round2-L30 复核·N126] 残余=整帧槽号常数平移 0x10（ORIG -0x14d… vs 我方 -0x15d…；帧
//   0x25c vs 0x24c）⇒ 判据⑪ (b) 类，声明重排无效；ORIG 0x08674968（0x115d）。
// [E04 2026-09-10 复核] p167 141/141 CLEAR（差集仅 C1/C2·D1/D2 别名）；另比较点
//   ORIG `mov -0x14b(%ebp),%eax; cmp $0x2061,%eax` vs 我方 `cmpl $0x2061,-0x58(%ebp)`
//   = N18b 比较形族（形参/局部 mem 直比）。N126+N18b 维持。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N95，gap=+19/blocks=228）→ 试=①prett/q3_gap2
//   缺口 0 ②n1_v：**帧长不同**（ORIG `sub $0x25c` vs 我方 `sub $0x24c`）+ 具名对象拷贝块
//   偏移整体平移（ORIG -0x218…-0x14d vs 我方 -0x15d…）⇒ 属 N115/N82 版本槽策略（4.4 块内
//   对象按序压深、4.1 反之）③句法旋钮不可补帧长差（若补假槽会引入额外指令反劣）
//   ⇒ 维持 N（1036/1017，gap +19）。
int CUser::multibox_lottery_item(int boxSlot, int keySlot)
{
    // b28d-solo-W8_CU_A：N 封顶（§6.2 巨件口径；原 b28d-35 判词沿用并复验）。
    //   ORIG @0x08674968 size 0x115d；P167 调用点差集 141/141 CLEAR（本轮复验）；
    //   帧 0x25c vs 我方 0x24c（差 0x10，首局部 boxItem -0x14d vs -0x15d），
    //   P210 把 CheckFailMultiBoxLottery 定义后置实测帧不变 ⇒ 非「已见定义
    //   callee」成因，属 N95/N82 槽序族；P282 复核 vtable 槽序无真 bug
    //   （vcall_cand 件：ORIG 0x16/0x20/0x12 各 1-3 处 vs 我方少 1）。
    Inven_Item boxItem;                                          // local_151
    boxItem = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, boxSlot);
    unsigned int boxItemId = static_cast<unsigned int>(boxItem.m_addInfo);

    std::vector<std::pair<Inven_Item, bool> > results;           // local_7c
    int ret = 0;                                                 // local_50
    if (boxItemId == 0x2061u || boxItemId == 0x289843u) {
        ret = process_use_unlocked_multibox(boxSlot, keySlot, results);
    } else {
        ret = process_use_locked_multibox(boxSlot, keySlot, results);
    }
    if (ret != 0) {
        if (ret == 0x13) {
            reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                ->send_error_state_packet(this, boxItemId);
            return -1;
        }
        return ret;
    }

    // [E04 单点 spec·F2a 已试·N 回滚] 按 ORIG 0x08674b56（`call; mov %al,-0x45(%ebp);
    //   cmpb $0x0,-0x45(%ebp); je`，1 字节直存无 test/setne）试
    //   `char isFirst = (char)call; if (isFirst != 0)`:横测 ours
    //   `_ZN5CUser21multibox_lottery_itemEii` **1017→1024（+7 劣化）**、TU 四数
    //   27/71/0/5 不变 ⇒ 4.4 对 char 局部 + `!=0` 反产符号扩展/回读链（ORIG 的
    //   `cmpb $0x0,槽` 单条不可复现）⇒ 按「gap 不改善即回滚」恢复 bool 形式。
    // [R20a15 §15B] cal_booster_gage 改 bool 后去掉 `!= 0`（ORIG 0x08674b51 段
    // 为单字节直存 `mov %al,-0x45(%ebp); cmpb $0x0,-0x45; je`，无 test/setne）。
    const bool isFirst =
        reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
            ->cal_booster_gage(boxItemId, this);
    if (isFirst) {
        reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
            ->log_booster_gage_bonus_reward(this, results);
    }

    PacketGuard out;                                             // local_88
    out.put_header(1, 0xd3);
    out.put_byte(1);
    out.put_short(boxSlot);
    out.put_short(keySlot);
    out.put_short(static_cast<int>(results.size()));

    std::vector<std::pair<int, int> > updatedSlots;              // local_94

    for (std::vector<std::pair<Inven_Item, bool> >::iterator it =
             results.begin();
         it != results.end(); ) {
        Inven_Item item = (*it).first;
        int newSlot = -1;                                        // local_48
        int remainOut = 0;                                       // local_a0
        unsigned int itemId = static_cast<unsigned int>(item.m_addInfo);

        if (itemId == 1u) {
            // ---- 事件金币项 ----
            int amount = item.m_addInfo2;                        // local_34 初值
            if (amount > 0 && amount < 1000) {
                int addAmount = amount;
                if (isFirst) {
                    addAmount = static_cast<int>(
                                    G_CDataManager()->m_boosterGage
                                        .m_cMultiboxCount) *
                                amount;
                }
                unsigned int coin =
                    getCurCharacInvenR()->GetEventCoin() +
                    static_cast<unsigned int>(addAmount);
                getCurCharacInvenW()->SetEventCoin(coin);
                coin = getCurCharacInvenR()->GetEventCoin();
                m_historyLog.EventCoinAdd(static_cast<int>(coin), addAmount,
                                          static_cast<eCoinAddReason>(2));
                SaveMoneyCoin();
                SendCoin();
                out.put_short(-1);
                out.put_int(item.m_addInfo);
                out.put_int(amount);
                out.put_short(static_cast<unsigned short>(item.m_endurance));
                out.put_byte(item.GetItemAttr());
                out.put_packet(item);
            }
            it++;                                                // ORIG 站点 1
            continue;
        }

        CItem* resItem = G_CDataManager()->find_item(itemId);    // local_38
        if (resItem == NULL) {
            return 0x11;
        }

        if (resItem->IsAvatarItem()) {
            char ipgBuf[24] = { 0 };                             // local_b8
            item.SetUpgrade(0);
            const char* ipg = WongWork::CGenUniqueNo::genIPGNo(
                WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
                static_cast<unsigned>(getCurCharacNo()), ipgBuf);
            newSlot =
                reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                    ->AddAvatarItem(updatedSlots, isFirst, this, item,
                                    resItem->getUsablePeriod(), false, '\0',
                                    static_cast<char>(-1), ipg,
                                    static_cast<eAvatarItemAddReason>(7),
                                    remainOut);
        } else if (resItem->IsCreatureItem() &&
                   resItem->GetItemType() == 0x16) {
            // ---- 生物蛋 ----
            if (item.m_itemType != '\x05') {
                newSlot = -1;
            } else if (
                user_creature::CCreatureItemConverter::SetEggItem(&item) == 1) {
                char ipgBuf[24] = { 0 };                         // local_d8+8
                const char* ipg = WongWork::CGenUniqueNo::genIPGNo(
                    WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
                    static_cast<unsigned>(getCurCharacNo()), ipgBuf);
                newSlot =
                    reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                        ->InsertCreatureItem(
                            updatedSlots, isFirst, this, item,
                            static_cast<eItemAddReason>(3), true, false,
                            resItem->getUsablePeriod(),
                            resItem->getExpirationDate(), ipg);
            } else {
                newSlot = -1;
            }
        } else {
            // ---- 普通 / 特殊商城品 ----
            int specialType = 0;                                 // local_d8[0]
            int specialValue = 0;                                // local_d8[1]
            bool specialFlag = false;                            // local_d9
            if (GlobalData::s_pCeraShop->IsSpecialItem(
                    itemId, specialType, specialValue, specialFlag)) {
                CItem* specItem = G_CDataManager()->find_item(itemId);
                // [R9a14 ④ #33] ORIG 0x086751aa/0x086751c1：`call *%edx` 两次（短路，
                //   首次返回 0x10 时不再调）且**无具名 kind 局部**；0x086751d4 test %al,
                //   0x086751d6 `je 0x08675396`（插入段落点）⇒ 原码是「单一判定 if/else」，
                //   主段 fall-through、插入段为跳转目标。原写「缓存 kind + 两处独立 if」
                //   使虚调用只发 1 次（ORIG 2 次）且多出第二处比较（我方 4 处 cmp vs ORIG 2 处）。
                if (!(specItem->GetItemType() == 0x10 ||
                      specItem->GetItemType() == 0x22)) {
                    int featured = 0;                            // local_e0
                    int restrictCode =
                        G_CDataManager()->m_itemList->GetRestrictCode(
                            itemId);
                    long curSec = GlobalData::s_systemTime_.getCurSec();
                    std::vector<Inven_Item> produceVec;          // local_ec
                    WongWork::CCeraShop::stCeraShopItemParam_t param;  // local_104
                    if (GlobalData::s_pCeraShop->ProcessSpecialItem(
                            this, itemId, restrictCode, featured, curSec, true,
                            produceVec, param, '\0', '\0', isFirst, 1)) {
                        if (featured != 0) {
                            newSlot = -1;
                        }
                        if (restrictCode != 0 &&
                            !IsRestrictedGoods(restrictCode)) {
                            SetRestrictedGoods(restrictCode);
                            GlobalData::s_pCeraShop->SaveFeaturedIdx(
                                this, restrictCode, static_cast<int>(curSec), 0);
                            SendRestrictedGoods(true, restrictCode);
                        }
                    }
                    newSlot = 0xffff;   // 特殊品不落背包槽（ORIG 写回 0xffff）
                }
                else {
                    newSlot =
                        reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                            ->insertItemIntoInventory(
                                updatedSlots, isFirst, this, item,
                                static_cast<eItemAddReason>(3), true);
                }
            } else {
                newSlot =
                    reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                        ->insertItemIntoInventory(
                            updatedSlots, isFirst, this, item,
                            static_cast<eItemAddReason>(3), true);
            }

            if (newSlot != -1) {
                remainOut = item.get_add_info();
                if (resItem == NULL) {
                    newSlot = -1;
                } else {
                    int price = 0;                               // local_108
                    // [R10a16 2026-09-12 · P200] ORIG 0x08675474 段为
                    // `mov -0x184(%ebp),%eax; movswl %ax,%ebx`（续读到 %ebx 跨
                    // 下一 call 保活）——short 拓宽**直传实参**、无具名中间局部；
                    // 我方原 `short count = ...` 多出 `mov %ax,-0x1a(%ebp);
                    // movswl -0x1a(%ebp),%ebx` 2 条 ⇒ 表达式内联回实参位。
                    G_Store()->GetSellItemPrice(
                        item, resItem,
                        static_cast<short>(item.m_addInfo2), false, price);
                    if (isFirst) {
                        price = price * static_cast<int>(
                                          G_CDataManager()->m_boosterGage
                                              .m_cMultiboxCount);
                    }
                    GetInstanceValueStatistic()->AddValueStatistic(
                        static_cast<VALUE_STATISTIC_FIELD>(7), this,
                        static_cast<unsigned int>(price));
                }
            }
        }

        // ORIG 两个分支各自完整重复发送序列（调用计数按站点复现）
        if (newSlot == -1) {
            out.put_short(-1);
            out.put_int(item.m_addInfo);
            out.put_int(item.get_add_info());
            out.put_short(static_cast<unsigned short>(item.m_endurance));
            out.put_byte(item.GetItemAttr());
            out.put_packet(item);
        } else {
            out.put_short(newSlot);
            out.put_int(item.m_addInfo);
            out.put_int(remainOut);
            out.put_short(static_cast<unsigned short>(item.m_endurance));
            out.put_byte(item.GetItemAttr());
            out.put_packet(item);
        }
        it++;                                                    // ORIG 站点 2
    }

    out.put_byte(isFirst ? 1 : 0);
    out.finalize(true);
    Send(out);

    for (std::vector<std::pair<int, int> >::const_iterator itUpd =
             updatedSlots.begin();
         itUpd != updatedSlots.end(); itUpd++) {
        SendUpdateItem(eSendTarget_1,
                       static_cast<ENUM_ITEMSPACE>(itUpd->second),
                       itUpd->first);
    }

    InGameADManagerInstance()->CheckFailMultiBoxLottery(this, results);

    for (std::vector<std::pair<Inven_Item, bool> >::iterator itShout =
             results.begin();
         itShout != results.end(); itShout++) {
        std::pair<Inven_Item, bool> entry = *itShout;
        if (entry.second) {
            PacketGuard shout;                                   // local_114
            shout.put_header(0, 0x56);
            shout.put_byte(4);
            shout.put_byte(1);
            shout.put_short(
                static_cast<unsigned short>(get_unique_id() & 0xffff));
            shout.put_int(entry.first.m_addInfo);
            shout.put_int(static_cast<int>(boxItemId));
            shout.put_byte(entry.first.GetUpgrade());
            shout.finalize(true);
            G_GameWorld()->send_all(shout);
        }
    }

    use_multibox_statistic(boxItemId);
    return 0;
}