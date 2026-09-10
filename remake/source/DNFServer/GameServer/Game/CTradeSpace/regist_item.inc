int CTradeSpace::regist_item(CUser* user, int type, int a, int b)
{
    // b28f-SweepCTSpace: DIFF ndiff=1280（O2021/N2019）。ORIG @0x08529e7e。
    if (IsLocked())
        return -0x63;
    if (b <= 0)
        return -0x62;

    // P331：ORIG 槽 opp -0x64（最深，+0x0e 预置零）/ other -0x60 / trader -0x5c
    // ⇒ 声明序 opp/other/trader，两分支 store 序 opp→other→trader。
    CUser* opp = 0;
    int other;
    int trader;
    if (m_pTraders[0] == user) {
        opp = m_pTraders[1];
        other = 1;
        trader = 0;
    } else {
        opp = m_pTraders[0];
        other = 0;
        trader = 1;
    }
    if (opp == 0)
        return -1;

    TradeSlot slot;
    slot.m_field08 = (char)type;

    if (user->getCurCharacR() == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x147, 5)
            ("CTradeSpace::regist_item()\t0 == owner->getCurCharacR()");
        return -2;
    }
    if (type != 0 && (user->isHackUser() || opp->isHackUser()))
        return -0x153;

    PacketGuard packet;

    // ORIG +0x144：switch 二分树（cmp1→type1 远跳、>1: 3/7、==0 fall），
    // 物理块序 case0 → case3(+0xcd7) → case1(+0x14c7) → case7(+0x1bcc) → default
    switch (type) {
    case 0: {
        if (a < 0 || a > 0x138) {
            LogManager::logFormat(
                1, "item_trade.cpp", __PRETTY_FUNCTION__, 0x15e,
                "slot_no(%d) < 0 || slot_no > Inven_Item::MAX_INVEN_SLOT", a);
            return -3;
        }
        if (a == 0) {
            // 金钱交易：反外挂总量规则 → 扣钱 → 入交易金额 → 发包
            if (GlobalData::g_autoPunishRuleHackTypeMgr->checkTotalTradeRuleOver(
                    user->getTotalHackUserTradeCnt(),
                    user->getTotalHackUserTradeGold()))
                return -0x16a;
            // P214①：ORIG +0x1e9 `call use_money; test %al,%al; je <远处 -5 块>`
            // （正极性、无 xor）⇒ 原码是正向包裹，非早返否定。
            if (user->getCurCharacInvenW()->use_money(b, (eMoneySubReason)2, 1)) {
                m_nMoney[trader] += b;
                if (checkCancelTrade() && checkTrade()) {
                    packet.clear();
                    packet.put_header(0, 0xf);
                    packet.put_short(a);
                    packet.put_int(0);
                    packet.put_int(m_nMoney[trader]);
                    packet.put_byte(0);
                    packet.put_short(0);
                    packet.put_int(0);
                    packet.put_byte(0);
                    packet.put_short(0);
                    packet.put_packet(g_emptySlot);  // ORIG +0x37a 直引全局
                    packet.finalize(true);
                    opp->Send(packet);
                    m_nState[trader] = 0;
                    m_nState[other] = 0;
                    return 0;
                }
                m_nMoney[trader] -= b;
                user->getCurCharacInvenW()->gain_money(b, (eMoneyAddReason)12, 1, 0);
                return -4;
            }
            return -5;
        }
        // 背包物品
        if (user->isHackUser() || opp->isHackUser())
            return -0x1a1;
        slot.m_item = user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, a);
        if (slot.m_item.m_addInfo == 0) {
            cMyTrace(__PRETTY_FUNCTION__, 0x1a8, 5)(
                "CTradeSpace::regist_item, item find failed(1), ch=%d %d %d %d",
                user->getCurCharacNo(), slot.m_item.m_addInfo,
                slot.m_item.get_add_info(), a);
        }
        if (!_IsTradable(slot.m_item))
            return -0x66;
        // ORIG +0x62f：field1!=1 时数量走 get_add_info() 调用比较
        if (slot.m_item.m_field1 != 1) {
            if (slot.m_item.get_add_info() < b) {
                cMyTrace(__PRETTY_FUNCTION__, 0x1b5, 5)(
                    "CTradeSpace::regist_item, item count failed(1), ch=%d %d %d %d",
                    user->getCurCharacNo(), slot.m_item.m_addInfo,
                    slot.m_item.get_add_info(), b);
                return -8;
            }
        }
        if (b <= 0) {
            cMyTrace(__PRETTY_FUNCTION__, 0x1be, 5)(
                "CTradeSpace::regist_item, item count failed(2), ch=%d %d %d %d",
                user->getCurCharacNo(), slot.m_item.m_addInfo,
                slot.m_item.get_add_info(), b);
            return -9;
        }
        if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, a, b, (eItemDelReason)2, 1)) {
            // P221/P227：全表达式临时链复现 ORIG 互斥块槽序（无具名对象）
            cMyTrace(__PRETTY_FUNCTION__, 0x1c8, 5)
                ("CTradeSpace::regist_item, delete_item failed, ch=%d %d %d",
                 user->getCurCharacNo(), a, b);
            return -10;
        }
        int slotIdx = add_item(trader, slot, b);
        if (slotIdx == -1)
            goto L_slot0;   // P325：ORIG 两条返回共享同一尾块（+0x2109）
        if (checkTrade() && checkCancelTrade()) {
            m_nState[trader] = 0;
            m_nState[other] = 0;
            packet.clear();
            packet.put_header(0, 0xf);
            packet.put_short(slotIdx);
            packet.put_int(slot.m_item.m_addInfo);
            packet.put_int(
                TradeSlotAt(m_slotsData, trader, slotIdx)
                    ->m_item.get_add_info());
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(slot.m_item.m_amp.getAbilityType());
            packet.put_short(slot.m_item.m_amp.getAbilityValue());
            packet.put_packet(slot.m_item);
            packet.finalize(true);
            opp->Send(packet);
            goto L_slot0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x1fc, 5)
            ("CTradeSpace::regist_item, (checkTrade() && checkCancelTrade()) "
             "failed, ch=%d %d %d",
             user->getCurCharacNo(), a, b);
        // 回滚：槽位扣减/清空后 insert_item_special_slot 归还。
        // insert_item_special_slot(item, count, slot)：ORIG 0x8529e7e 处实参为
        // (param_4 数量, param_3 原槽)（汇编 esp+0x44=param_4、esp+0x48=param_3）。
        // ORIG +0xa8f-0xb93：equipable→reset(+0xa9b)；非 equipable：
        // ==b→reset(+0xb95 尾块)、>b→重读链+写回链(+0xaf5/+0xb29/+0xb4b)、
        // <b→reset(+0xb6a)。4.1 无 CSE，条件/读/写各独立全链；两 reset
        // 代码点不合并。
        if (TradeSlotAt(m_slotsData, trader, slotIdx)
                ->m_item.isEquipableItemType()) {
            TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        } else if (TradeSlotAt(m_slotsData, trader, slotIdx)
                       ->m_item.m_addInfo2 == b) {
            TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        } else if (TradeSlotAt(m_slotsData, trader, slotIdx)
                       ->m_item.m_addInfo2 > b) {
            int cnt = TradeSlotAt(m_slotsData, trader, slotIdx)
                          ->m_item.m_addInfo2;
            TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_addInfo2 =
                cnt - b;
        } else {
            TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        }
        if (user->getCurCharacInvenW()->insert_item_special_slot(slot.m_item, b, a,
                                            (eMoneyAddReason)9,
                                            (eItemAddReason)0x10) < 0) {
            user->m_historyLog.TradeItemAddFail(slot.m_item.m_addInfo, b);
            return -11;
        }
        return -12;
    L_slot0:
        return slotIdx;
    }
    case 3: {
        // 装备栏（0..0x19）
        if (a < 0 || a > 0x19) {
            LogManager::logFormat(
                1, "item_trade.cpp", __PRETTY_FUNCTION__, 0x266,
                "slot_no(%d) < 0 || slot_no > ENUM_EQUIPMENTTYPE_TOTAL_MAX", a);
            return -13;
        }
        slot.m_item = user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_INVEN, a);
        if (slot.m_item.m_addInfo == 0)
            return -14;
        if (!_IsTradable(slot.m_item))
            return -0x66;
        if (a <= 9) {
            // 头部槽：临时 key / 过期校验
            if (WongWork::CAvatarItemMgr::IsTempKey(slot.m_item.m_addInfo2))
                return -0x66;
            slot.m_field00 = a;
            slot.m_field04 = user->getCurCharacNo();
            int expire =
                user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpireDate(slot.m_item.m_addInfo2);
            if (expire != 0 && GlobalData::s_systemTime_.getCurSec() > expire)
                return -0x64;
        }
        if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_INVEN, a, 1, (eItemDelReason)2, 1)) {
            cMyTrace(__PRETTY_FUNCTION__, 0x281, 5)
                ("CTradeSpace::regist_item 2, delete_item failed, ch=%d, %d %d",
                 user->getCurCharacNo(), a, b);
            return -15;
        }
        int slotIdx = add_item(trader, slot, b);
        if (slotIdx == -1)
            goto L_slot3;   // P325：共享尾块
        if (checkTrade() && checkCancelTrade()) {
            m_nState[trader] = 0;
            m_nState[other] = 0;
            packet.clear();
            packet.put_header(0, 0xf);
            packet.put_short(slotIdx);
            packet.put_int(slot.m_item.m_addInfo);
            // ORIG +0x1098-0x1120：tick 先求值 → 槽链 get_add_info()（非直读）
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8) {
                int tick = OS_API::GetDateTimeTick();
                int ai = TradeSlotAt(m_slotsData, trader, slotIdx)
                             ->m_item.get_add_info();
                packet.put_int(
                    user->getCurCharacInvenR()->GetAvatarItemMgrR()
                        ->GetRemainDate(ai, tick));
            } else {
                packet.put_int(
                    TradeSlotAt(m_slotsData, trader, slotIdx)
                        ->m_item.get_add_info());
            }
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(slot.m_item.m_amp.getAbilityType());
            packet.put_short(slot.m_item.m_amp.getAbilityValue());
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8) {
                packet.put_int(0x1e);
                packet.put_binary(user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                                      TradeSlotAt(m_slotsData, trader, slotIdx)
                                          ->m_item.m_addInfo2),
                                  0x1e);
                packet.put_int(4);
                stAvatarExpansionInfo_t info;
                // +0x12a2：addInfo2 重读链（4.1 无 CSE）
                void* pExp = user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                    TradeSlotAt(m_slotsData, trader, slotIdx)
                        ->m_item.m_addInfo2);
                if (pExp != 0)
                    packet.put_binary((const char*)pExp, 4);
                else {
                    info.init();
                    packet.put_binary((const char*)&info, 4);
                }
            }
            packet.put_packet(slot.m_item);
            packet.finalize(true);
            opp->Send(packet);
            goto L_slot3;
        }
        // 回滚：insert_item_special_slot 归还。
        // ORIG 0x8529e7e（type==3 失败路径）实参 = (slot.m_item, param_4, param_3)，
        // 即 (数量=用户提交 b, 槽位=原装备槽 a)——与 type0 同布局（esp+0x44/0x48）。
        TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        if (user->getCurCharacInvenW()->insert_item_special_slot(slot.m_item, b, a,
                                            (eMoneyAddReason)9,
                                            (eItemAddReason)0x10) < 0) {
            user->m_historyLog.TradeItemAddFail(slot.m_item.m_addInfo, b);
            return -16;
        }
        return -17;
    L_slot3:
        return slotIdx;
    }
    case 1: {
        // 头像
        slot.m_item = user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, a);
        if (b <= 0)
            return -9;
        slot.m_field00 = a;
        slot.m_field04 = user->getCurCharacNo();
        int expire =
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpireDate(slot.m_item.m_addInfo2);
        if (expire != 0 && GlobalData::s_systemTime_.getCurSec() > expire)
            return -0x64;
        if (WongWork::CAvatarItemMgr::IsTempKey(slot.m_item.m_addInfo2))
            return -0x66;
        if (!_IsTradable(slot.m_item))
            return -0x66;
        for (int i = 3; i <= 0x1a; ++i) {
            if (TradeSlotAt(m_slotsData, trader, i)->m_item.m_addInfo2 ==
                slot.m_item.m_addInfo2)
                return -0x67;
        }
        if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_AVATAR, a, b, (eItemDelReason)1, 1))
            return -10;
        int slotIdx = add_item(trader, slot, b);
        if (slotIdx == -1)
            goto L_slot1;   // P325：共享尾块
        if (checkTrade() && checkCancelTrade()) {
            m_nState[trader] = 0;
            m_nState[other] = 0;
            packet.clear();
            packet.put_header(0, 0xf);
            packet.put_short(slotIdx);
            packet.put_int(slot.m_item.m_addInfo);
            {
                // ORIG +0x15xx：tick → 槽链 get_add_info() → mgrR（求值序）
                int tick = OS_API::GetDateTimeTick();
                int ai = TradeSlotAt(m_slotsData, trader, slotIdx)
                             ->m_item.get_add_info();
                packet.put_int(
                    user->getCurCharacInvenR()->GetAvatarItemMgrR()
                        ->GetRemainDate(ai, tick));
            }
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(0);
            packet.put_short(0);
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8) {
                packet.put_int(0x1e);
                packet.put_binary(user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                                      TradeSlotAt(m_slotsData, trader, slotIdx)
                                          ->m_item.m_addInfo2),
                                  0x1e);
                packet.put_int(4);
                stAvatarExpansionInfo_t info;
                void* pExp = user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                    TradeSlotAt(m_slotsData, trader, slotIdx)
                        ->m_item.m_addInfo2);
                if (pExp != 0)
                    packet.put_binary((const char*)pExp, 4);
                else {
                    info.init();
                    packet.put_binary((const char*)&info, 4);
                }
            }
            packet.put_packet(slot.m_item);
            packet.finalize(true);
            opp->Send(packet);
            goto L_slot1;
        }
        // 回滚：insertAvatarIntoSpecificSlot 归还
        TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        if (user->getCurCharacInvenW()->insertAvatarIntoSpecificSlot(slot.m_item, a,
                                                (eAvatarItemAddReason)4, 1) < 0) {
            user->m_historyLog.TradeItemAddFail(slot.m_item.m_addInfo, b);
            return -11;
        }
        return -12;
    L_slot1:
        return slotIdx;
    }
    case 7: {
        // 宠物
        slot.m_item = user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_CREATURE, a);
        if (b <= 0)
            return -9;
        slot.m_field00 = a;
        slot.m_field04 = user->getCurCharacNo();
        if (!_IsTradable(slot.m_item))
            return -0x66;
        if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_CREATURE, a, b, (eItemDelReason)0x15, 1))
            return -10;
        int slotIdx = add_item(trader, slot, b);
        if (slotIdx == -1)
            goto L_slot7;   // P325：共享尾块
        if (checkTrade() && checkCancelTrade()) {
            m_nState[trader] = 0;
            m_nState[other] = 0;
            packet.clear();
            packet.put_header(0, 0xf);
            packet.put_short(slotIdx);
            packet.put_int(slot.m_item.m_addInfo);
            packet.put_int(
                TradeSlotAt(m_slotsData, trader, slotIdx)
                    ->m_item.get_add_info());
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(0);
            packet.put_short(0);
            if (slot.m_item.m_field1 == 5) {
                // ORIG：tick 先行局部、add_info 走槽链 call
                // P198③：ORIG +0x1f16 `movl $0x0,-0x24` 后 `call tick` 回写
                // ⇒ 原码是「声明=0 + 独立赋值」两句式。
                int tick = 0;
                tick = OS_API::GetDateTimeTick();
                int ai = TradeSlotAt(m_slotsData, trader, slotIdx)
                             ->m_item.get_add_info();
                packet.put_int(
                    user->getCurCharacInvenR()->GetCreatureMgrR()
                        ->GetRemainDate(ai, tick));
            }
            packet.put_packet(slot.m_item);
            packet.finalize(true);
            opp->Send(packet);
            goto L_slot7;
        }
        // 回滚：insertCreatureIntoSpecificSlot 归还
        TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        if (user->getCurCharacInvenW()->insertCreatureIntoSpecificSlot(slot.m_item, a,
                                                  (eItemAddReason)0x1d, 1) < 0) {
            user->m_historyLog.TradeItemAddFail(slot.m_item.m_addInfo, b);
            return -11;
        }
        return -12;
    L_slot7:
        return slotIdx;
    }
    default:
        return -0x14;
    }
