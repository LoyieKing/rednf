// ===========================================================================
// 6) CUser::lottery_item(int slot) @0x086733a2 —— 使用槽上 lottery 物品开奖。
//    金币奖励 / avatar / 生物蛋 / 强化三档概率（ServerParameterScript 内嵌表）/ 
//    useCost 扣款通知 / 主响应包 / 全服强化播报。全程照报告转写。
//    §6.2 巨件核销：P167 200/200 [0缺0多]；gap 1315→1330；残差归因 N 类
//    （N82 全帧槽平移 0x18/0x10 + 槽复用差、N-88/setne 物化假象（ORIG
//    cmpl/setne/test vs 4.4 cmp 直跳，P197④ 探针同型）、N22 比较方向反演
//    （get_rand_int 三段 seta↔setb 同语义折向）、bool 聚合变量槽物化
//    （movb/-0x51 槽 vs ORIG 寄存器 test）、short 形参槽中转（movswl 经
//    -0x22 中转）——语义与调用序列逐块比对零缺口，byte 层 N 封顶。
// ===========================================================================
// [round2-L30 复核·N95/N82] shift.py: frame ORIG=780 OURS=780，MIXED（相对序差，含 8-dword
//   结构拷贝展开位）⇒ 大件未改造，判 N；ORIG 0x086733a2（0x15c6）。
// [E04 2026-09-10 复核] p167 200/200 CLEAR（差集全为 C1/C2·D1/D2 同址别名 P93）⇒ 无真缺口；
//   gap +15 维持 N82/N95 槽序族。
// [F2a-C05② 复测·不适用] 件级 F2 旋钮逐条核对报告 5 处 ORIG `cmp -0xNN(%ebp),%eax`：
//   ① ORIG 0x086735... `cmp -0x40(%ebp),%eax; jle`（(prev+delta) vs 局部）我方对应点
//      已是同形 mem 直比（`cmp -0x4c(%ebp),%eax; setg`，仅槽号整体差 0xc，属 N82）；
//   ② `cmp -0x44(%ebp),%eax`（ORIG 0x086736xx）与 `cmp -0x30(%ebp),%eax; seta`
//      （ORIG 0x08673xx）的 mem 操作数是 **call 返回值局部**（gain_money / get_rand_int
//      的结果槽）⇒ C05② 边界（跨调用返回值不适用），且后者为无符号序 seta；
//   ③ 无一处属「两个具名局部比较」命中形 ⇒ 维持 N82 封顶，无改动、无回滚。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N22/N82/N95，gap=-15/blocks=247）→ 试=①prett2/
//   q3_gap2 缺口 0、strall 串 CLEAR（仅 base64 假阳性）②4.4.6 后端 overlay 复编对该 TU
//   无益（同族先例：W7_LinkTailGrowth 446/447 四数逐项相同）③n1_v 首残差 = 帧内**类对象
//   槽整体平移**（ORIG -0x5c/-0x139/-0x176/-0x2b8 vs 我方 -0x74/-0x151/-0x18e/…，帧同为
//   `sub $0x30c`）⇒ 4.1/4.4 按首次使用序分配局部对象的槽策略差（N82/N95），非 P339 声明序
//   可造（帧长已全等）⇒ 维持 N（1315/1330，gap −15 为版本槽策略）。
int CUser::lottery_item(int slot)
{
    // b28d-solo-W8_CU_A：N 封顶（§6.2 巨件口径；原 b28d-35 判词沿用并复验）。
    //   ORIG @0x086733a2 size 0x15c6；P167 调用点差集 200/200 CLEAR（本轮复验）；
    //   帧 0x30c 与 ORIG 相同，但 4.4 槽分配整体浅移 0x18（resultSlotNo -0x74
    //   vs ORIG -0x5c、lotto -0x151 vs -0x139、curItem -0x18e vs -0x176）——
    //   属 N82/N95 槽序族；残余另含 setne 物化（N-88）与 N22 比较折向。
    int resultSlotNo = 0;                                        // local_60
    Inven_Item lotto;                                            // local_13d
    Inven_Item curItem;                                          // local_17a
    curItem = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    unsigned int ticketId = static_cast<unsigned int>(curItem.m_addInfo);

    CItem* costItem = G_CDataManager()->find_item(ticketId);     // local_58
    if (costItem == NULL) {
        return 0x15;
    }
    if (costItem->getUsableLevel() > get_charac_level()) {
        return 0xe;
    }

    // NOTE(NEEDS_FIX)：find_lottery_use_cost 见文件头注记
    int useCost = G_CDataManager()->find_lottery_use_cost(ticketId);  // local_50
    if (useCost != 0 && getCurCharacInvenR()->get_money() < useCost) {
        return 10;
    }

    G_CDataManager()->m_itemList->get_lotto_item(
        ticketId, reinterpret_cast<char*>(&lotto));

    // ---- 金币奖励分支：结果项无 id 且金额>0 ----
    if (lotto.m_addInfo == 0 && lotto.m_addInfo2 > 0) {
        int moneyGain = lotto.m_addInfo2;                        // local_44
        // [R20a19 ④] ORIG 0x086735b7 `movl $0x7fffffff,-0x40(%ebp)`：原码
        //   声明处带初值 INT_MAX（随后被 GetMoneyLimitPerLevel 结果覆盖，两写点
        //   0x086735b7/0x086735ed —— P348「原码有初值必须照抄」）。我方原合写成
        //   单条初始化 ⇒ 少该 store（+1 指令）。
        int limit = 0x7fffffff;                                  // local_40
        limit = G_CDataManager()->GetMoneyLimitPerLevel(
            get_charac_level(), get_acc_name());
        if (getCurCharacInvenR()->get_money() + moneyGain > limit) {
            unsigned long charNo = getCurCharacNo();
            const char* accIdStr = NumberToString(get_acc_id(), 0);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x61b9, 4);
            trace("lottery_gold_item:m_id(%s), char_no(%d), money(%d)",
                  accIdStr, charNo, moneyGain);
            return 0x16;
        }
        int gained = getCurCharacInvenW()->gain_money(
            moneyGain, static_cast<eMoneyAddReason>(0x17), true, 0);  // local_3c
        if (gained < moneyGain) {
            SendMoneyFullReason(static_cast<ENUM_MONEY_FULL_REASON>(0),
                                static_cast<unsigned long>(moneyGain),
                                static_cast<unsigned long>(gained));
        }
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                               eItemDelReason_3, true)) {
            return 0x11;
        }
        PacketGuard out;                                         // local_bc
        out.put_header(1, 0x1d);
        out.put_byte(1);
        out.put_short(slot);
        out.put_short(resultSlotNo);
        out.put_int(0);
        out.put_int(gained);
        out.put_short(0);
        out.put_byte(0);
        out.put_byte(0);
        out.put_short(0);
        out.put_packet(g_emptySlot);
        out.finalize(true);
        Send(out);
        send_broadcast_lottery_gold_message(static_cast<unsigned long>(ticketId),
                                            moneyGain);
        return 0;
    }

    // ---- 结果道具抽取强化概率（costTicket 可堆叠且类型 0xd 时进入；
    //      SPS 字段 dm+0x5xx ↔ m_serverParameter(基 0x68) 相对偏移映射见头注）----
    unsigned int resultId = static_cast<unsigned int>(lotto.m_addInfo);
    CItem* costTicket = G_CDataManager()->find_item(ticketId);   // local_54(栈-0x54)
    CItem* resItem = G_CDataManager()->find_item(resultId);      // local_50(栈-0x50)

    // ORIG 条件精写（报告 0x051c..0x05d2）：
    if (costTicket != NULL && resItem != NULL) {
        bool condStack = costTicket->is_stackable() &&
                         costTicket->GetItemType() == 0xd;
        if (condStack) {
            bool upgradeGate =
                (lotto.m_field1 == '\x01') &&
                (resItem->GetItemType() != 0xb);
            if (upgradeGate) {
                int up = 0;                                      // local_34
                int roll = get_rand_int(10000);                  // local_30
                if (static_cast<unsigned int>(roll) <
                    static_cast<unsigned int>(
                        G_CDataManager()->m_serverParameter.m_508[10])) {
                    up = G_CDataManager()->m_serverParameter.m_508[8] +
                         get_rand_int(
                             G_CDataManager()->m_serverParameter.m_508[11]) /
                         1000;
                } else if (static_cast<unsigned int>(roll) <
                           static_cast<unsigned int>(
                               G_CDataManager()->m_serverParameter.m_508[14] +
                               G_CDataManager()->m_serverParameter.m_508[10])) {
                    up = G_CDataManager()->m_serverParameter.m_508[12] +
                         get_rand_int(
                             G_CDataManager()->m_serverParameter.m_508[15]) /
                         1000;
                } else {
                    up = G_CDataManager()->m_serverParameter.m_508[16] +
                         get_rand_int(
                             G_CDataManager()->m_serverParameter.m_508[19]) /
                         1000;
                }
                if (up < 0 || up > 0x1f) {
                    up = 0;
                }
                lotto.SetUpgrade(static_cast<unsigned char>(up));
            }
        }
    }

    if (resultId == 0u || resultId == 0xffffffffu) {
        return 0x11;
    }

    // ---- 插入容量预检 ----
    if (getCurCharacInvenR()->QuickEmptyCount() <= 0) {
        if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE_EQUIP,
                                                     1)) {
            return 4;
        }
        if (curItem.get_add_info() >= 2) {
            if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE_2,
                                                         1)) {
                return 4;
            }
        }
        if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE_3,
                                                     1)) {
            return 4;
        }
        if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE_10,
                                                     1)) {
            return 4;
        }
    }

    // ---- 宠物罐子票（0x28bfa1）：读 4 个罐子槽，任一强化等级 >9 拒绝 ----
    if (ticketId == 0x28bfa1u) {
        Inven_Item jar1;
        getCurCharacInvenW()->GetInvenData(0x28bf9d, jar1);
        Inven_Item jar2;
        getCurCharacInvenW()->GetInvenData(0x28bf9e, jar2);
        Inven_Item jar3;
        getCurCharacInvenW()->GetInvenData(0x28bf9f, jar3);
        Inven_Item jar4;
        getCurCharacInvenW()->GetInvenData(0x28bfa0, jar4);
        if (jar1.m_addInfo2 > 9 || jar2.m_addInfo2 > 9 ||
            jar3.m_addInfo2 > 9 || jar4.m_addInfo2 > 9) {
            return 4;
        }
    }

    // ---- 消耗票（id != 0x28becb 即刻删除；否则延后） ----
    if (ticketId != 0x28becbu) {
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                               eItemDelReason_3, true)) {
            return 0x11;
        }
    }

    int remainOrAdd = 0;                                         // local_2c

    if (resItem != NULL && resItem->IsAvatarItem()) {
        // ---- avatar 分支 ----
        char ipgBuf[24];                                         // local_e0
        const char* ipg = WongWork::CGenUniqueNo::genIPGNo(
            WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
            static_cast<unsigned>(getCurCharacNo()), ipgBuf);
        resultSlotNo = getCurCharacInvenW()->AddAvatarItem(
            static_cast<int>(resultId), resItem->getUsablePeriod(), false,
            '\0', static_cast<char>(-1), ipg,
            static_cast<eAvatarItemAddReason>(7), false, 0);
        if (resultSlotNo == -1) {
            return 4;
        }
        int nowSec = GlobalData::s_systemTime_.getCurSec();      // local_24
        int avatarUid = getCurCharacInvenR()->GetInvenSlot(
            INVEN_TYPE_AVATAR, resultSlotNo).m_addInfo2;
        remainOrAdd = getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
            avatarUid, nowSec);
        lotto.SetUpgrade(0);
    } else if (resItem != NULL && resItem->IsCreatureItem() &&
               resItem->GetItemType() == 0x16) {
        // ---- 生物蛋分支 ----
        if (lotto.m_field1 != '\x05') {
            return 0x11;
        }
        if (user_creature::CCreatureItemConverter::SetEggItem(&lotto) != 1) {
            return 0x11;
        }
        resultSlotNo = getCurCharacInvenW()->insertItemIntoInventory(
            lotto, static_cast<eItemAddReason>(3), true, false);
        if (resultSlotNo < 0) {
            return 4;
        }
        getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
            &lotto, resultSlotNo, 0x19, resItem->getUsablePeriod(),
            resItem->getExpirationDate());
    } else {
        // ---- 普通道具（含随机选项生成）----
        // [R9a20 2026-09-12 ④] 删除 `resItem != NULL &&` 防御性判空（复刻 ORIG）：
        //   ORIG 0x08674062 `mov -0x50(%ebp),%eax; mov (%eax),%eax; add $0x4c,%eax`
        //   直接解引用（无 cmpl/je）；我方 4.4 为判空多发 `cmpl $0x0,-0x5c(%ebp); je`
        //   （改前 .o 0x10c6 实测）。删除后新 .o 0x10c6 段与 ORIG 逐条同形。
        if (resItem->IsRandomOption()) {
            if (!GlobalData::g_randomOptionHandle->give_option(
                    resultId,
                    static_cast<ENUM_RARITY>(resItem->get_rarity()),
                    resItem->getUsableLevel(), resItem->getItemGroupName(),
                    resItem->GetRandomOptionGrade(), &lotto.m_random.m_random)) {
                return 0x11;
            }
        }
        resultSlotNo = getCurCharacInvenW()->insertItemIntoInventory(
            lotto, static_cast<eItemAddReason>(3), true, true);
        if (resultSlotNo == -1) {
            return 4;
        }
        if (ticketId == 0x28becbu) {
            if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                                   eItemDelReason_3, true)) {
                return 0x11;
            }
        }
        remainOrAdd = lotto.get_add_info();

        if (resItem != NULL) {
            int sellPrice = 0;                                   // local_e4
            short count = static_cast<short>(lotto.m_addInfo2);
            G_Store()->GetSellItemPrice(lotto, resItem, count, false,
                                        sellPrice);
            GetInstanceValueStatistic()->AddValueStatistic(
                static_cast<VALUE_STATISTIC_FIELD>(7), this,
                static_cast<unsigned int>(sellPrice));
        }
    }

    // ---- 老 lottery 物品清行动点 ----
    if (IsOldLotteryItem(ticketId)) {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, static_cast<APSystem::_ActionGroupIndex>(4), 0,
            static_cast<ENUM_PACKETCLASS>(0));
    }

    // ---- useCost 二次扣款与货币更新包 ----
    useCost = G_CDataManager()->find_lottery_use_cost(ticketId);
    if (useCost != 0) {
        getCurCharacInvenW()->use_money(
            useCost, static_cast<eMoneySubReason>(0x2d), true);
        PacketGuard notify;                                      // local_f0
        notify.clear();
        notify.put_header(0, 0xe);
        notify.put_byte(0);
        notify.put_short(1);
        getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_EQUIP, 0, notify);
        notify.finalize(true);
        Send(notify);
    }

    // ---- 主响应包 ----
    PacketGuard resp;                                            // local_c8
    resp.put_header(1, 0x1d);
    resp.put_byte(1);
    resp.put_short(slot);
    resp.put_short(resultSlotNo);
    resp.put_int(static_cast<int>(resultId));
    resp.put_int(remainOrAdd);
    resp.put_short(static_cast<unsigned short>(lotto.m_fieldb));
    resp.put_byte(lotto.GetItemAttr());
    resp.put_byte(lotto.m_amp.getAbilityType());
    resp.put_short(lotto.m_amp.getAbilityValue());
    if (lotto.isAvatarItemType()) {
        resp.put_int(0x1e);
        resp.put_binary(getCurCharacInvenR()->GetAvatarItemMgrR()
                            ->getJewelSocketData(
                                static_cast<int>(lotto.m_addInfo2)),
                        0x1e);
        resp.put_int(4);
        const char* expansion = static_cast<const char*>(
            getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                static_cast<int>(lotto.m_addInfo2)));
        if (expansion != NULL) {
            resp.put_binary(expansion, 4);
        } else {
            stAvatarExpansionInfo_t fresh;
            fresh.init();
            resp.put_binary(reinterpret_cast<const char*>(&fresh), 4);
        }
    }
    resp.put_packet(lotto);
    resp.finalize(true);
    Send(resp);

    if (lotto.GetUpgrade() != 0) {
        m_historyLog.UpgradeItem(static_cast<int>(resultId), 0,
                                 lotto.GetUpgrade(), true);
    }

    // ---- 强化成功全服播报（含稀度上限豁免表）----
    // [R20a19 ④/§26.3] ORIG 0x08674748..0x08674813 为**寄存器物化**短路链
    //   （`mov $0x1/%0,%eax; test %al,%al; je`，全函数 0 处 movb ⇒ 原码无具名
    //   bool 局部）；我方原具名 bool 多出 1 条预清 + 2 条置位（movb ×3）⇒ 按
    //   「我方多出的物化一律移除」改为单条短路表达式直作 if 条件。
    if (costTicket != NULL && resItem != NULL &&
        costTicket->is_stackable() && costTicket->GetItemType() == 0xd &&
        (G_CDataManager()->m_serverParameter.m_558 <=
             resItem->get_rarity() ||                          // 高稀度直接播报
         (resItem->IsAvatarItem() &&
          G_CDataManager()->m_serverParameter
              .m_55c[resItem->GetItemType()] != 0) ||          // 表命中 avatar 特例
         G_CDataManager()->m_serverParameter.m_noticeLegacyWithUpgrade <=
             static_cast<int>(lotto.GetUpgrade())))            // 强化等级门槛
    {
        // [R9a20 2026-09-12 ④] 原 `(resItem != NULL && resItem->IsRandomOption())` 判空
        //   已删除：ORIG 0x086747e9 `cmp $0x1,%eax` / `je` 后直接
        //   `0x086747ee mov -0x50(%ebp),%eax; 0x086747f1 mov (%eax),%eax`（无判空）；
        //   我方改前多发 `cmpl $0x0,-0x5c(%ebp); je`（.o 0x184f）。删除后新 .o 0x184f
        //   段与 ORIG 同形；函数前向 jcc 数 75→72 = ORIG。
        bool skipByPlace =
            (getMoveSpace() == static_cast<UserSpace::T>(1)) ||
            (resItem->IsRandomOption());
        if (!skipByPlace) {
            PacketGuard shout;                                    // local_100
            shout.put_header(0, 0x56);
            shout.put_byte(2);
            shout.put_byte(1);
            shout.put_short(
                static_cast<unsigned short>(get_unique_id() & 0xffff));
            shout.put_int(static_cast<int>(resultId));
            shout.put_byte(lotto.GetUpgrade());
            shout.finalize(true);
            G_GameWorld()->send_all(shout);
        }
    }
    return 0;
}