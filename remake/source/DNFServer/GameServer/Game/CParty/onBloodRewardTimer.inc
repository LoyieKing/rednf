// ============================================================================
// CParty::onBloodRewardTimer —— ORIG 0x85b6a48：0x22/0x23/0x24/0x25 血轮
// 定时器状态机（奖励包→物品/经验发放→收尾）。
// ============================================================================
// §6.2 巨件核销：P167 88/88 [0缺0多]；gap 766→46；残差归因 N82（帧 0x1cc vs 0x1ec
// 全帧槽基错位）+ P168a③b（checkValidUser bool callee 调用侧 xor$1 物化，跨版本
// 不可翻）+ 分支布局镜像（jne/je 块序）。
// 真修：0x24 分支体内 8 处 get_user(slot) → m_member[slot].m_pUser 内联读
//（ORIG 仅两处循环条件 call get_user，体内 11 处 add $0x78 数组寻址，EXTRA +8）；
// 删 `CBattle_Field& bf` 缓存槽（P183），bf 21 处调用展开 m_battleField 直调。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x1cc/0x1ec；720 vs 763
bool CParty::onBloodRewardTimer(TIMER_MESSAGE msg)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b6a48
    if (gen_timer_key(msg) == 0)
        return 0;
    if (m_battleField.getBloodState() == 0)
        return 0;

    if ((int)msg == 0x23)
    {
        if (m_battleField.getBloodState() < 3)
            return 0;
        for (int slot = 0; slot < 4; ++slot)            // -0x3c
        {
            if (!(get_user(slot) == 0 || !checkValidUser(slot)))
            {
                // ORIG onBloodRewardTimer.md +0x1c5..+0x214 装载序：param_3=
                // count(0)…param_10=item(3)，0→3 升序（先 group0 后 group3）。
                BloodClearRewardPacket(
                    slot,
                    m_battleField.m_bloodReward.getCurrentBloodRound(),
                    m_battleField.m_bloodReward.getRewardItemCount(0),
                    m_battleField.m_bloodReward.getRewardItem(0),
                    m_battleField.m_bloodReward.getRewardItemCount(1),
                    m_battleField.m_bloodReward.getRewardItem(1),
                    m_battleField.m_bloodReward.getRewardItemCount(2),
                    m_battleField.m_bloodReward.getRewardItem(2),
                    m_battleField.m_bloodReward.getRewardItemCount(3),
                    m_battleField.m_bloodReward.getRewardItem(3));
            }
        }
        m_battleField.setBloodState((ENUM_BLOOD_STATE)4);
        // [G7-sub] P0 槽律：ORIG 0x085b6c9d `call gen_timer_key` 后 `mov %eax,%esi`
        // （0x085b6cca `mov %esi,0x14(%esp)`）—— 结果**无栈槽** ⇒ 原码为实参表达式，
        // 无 `int key` 具名量；我方原 -0x4c 槽（帧 +0x10 之一）已删。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x24, 8,
                                    gen_timer_key((TIMER_MESSAGE)0x24), 0x24);
    }
    else if ((int)msg == 0x24)
    {
        if (m_battleField.getBloodState() < 4)
            return 0;
        for (int slot = 0; slot < 4; ++slot)            // -0x38
        {
            if (!(get_user(slot) == 0 || !checkValidUser(slot)))
            {
                for (int i = 0; i < m_battleField.m_bloodReward.getRewardItemCount(slot);
                     ++i)                               // -0x30
                {
// §P331 第二形：ORIG 把 getRewardItem 返回基址存 -0x28(%ebp) 后，每次字段访问都重算
// `mov -0x2c(%ebp),%eax; imul $0x3d,%eax,%eax; add -0x28(%ebp),%eax`（共 12 处；本方原
// 1 —— 被 `item` 一步起基址吸收）。拆成基址存槽 + 宏，即逐次重物化；i 为循环变量、
// 体内不改 i ⇒ 寻址同物，无可观察行为变化。实测 599→543。
                    Inven_Item* rewardBase = m_battleField.m_bloodReward.getRewardItem(slot);
#define item (rewardBase + i)
                    int putSlot = -1;                   // -0x28
                    // ORIG onBloodRewardTimer.md 085b6d92/98 `imul $0x3d +
                    // mov 0x2(%eax)`：Inven_Item(sizeof 0x3d)+0x02 → m_addInfo
                    if (item->m_addInfo != -1)
                    {
                        if (item->m_addInfo == 0)   // 金币槽
                        {
                            int gained =
                                reinterpret_cast<CUserCharacInfo*>(
                                    m_member[slot].m_pUser)
                                    ->getCurCharacInvenW()
                                    ->gain_money(item->get_add_info(),
                                                 (eMoneyAddReason)5, true,
                                                 false);
                            if (gained < item->get_add_info())
                                m_member[slot].m_pUser->SendMoneyFullReason(
                                    (ENUM_MONEY_FULL_REASON)0,
                                    (unsigned long)item->get_add_info(),
                                    (unsigned long)gained);
                            putSlot = 0;
                            // ORIG 085b6e88 `movzbl 0x89f(%eax)` =
                            // CDungeon::m_bloodDungeonType（unsigned char 宽度修正）
                            if (m_battleField.m_pDungeon->m_bloodDungeonType == 1)
                                GetInstanceCirculationStatistic()
                                    ->AddCirculationStatistic(
                                        (CIRCULATION_STATISTIC_FIELD)0xb,
                                        m_member[slot].m_pUser,
                                        (unsigned int)gained);
                            // ORIG 085b6ed8 `movzbl 0x89f(%eax)`（同上）
                            else if (m_battleField.m_pDungeon->m_bloodDungeonType == 2)
                                GetInstanceCirculationStatistic()
                                    ->AddCirculationStatistic(
                                        (CIRCULATION_STATISTIC_FIELD)0xc,
                                        m_member[slot].m_pUser,
                                        (unsigned int)gained);
                        }
                        else
                        {
                            putSlot =
                                reinterpret_cast<CUserCharacInfo*>(
                                    m_member[slot].m_pUser)
                                    ->getCurCharacInvenW()
                                    ->insertItemIntoInventory(
                                        *item, (eItemAddReason)0xb, true,
                                        true);
                        }
                    }
                    if (putSlot < 0)
                    {
                        // Inven_Item::m_addInfo（同上 085b6dae/366 站点）
                        int itemKey = item->m_addInfo;
                        if (itemKey == 0x4e3 || itemKey == 0x4e4)
                        {
                            if (G_CDataManager()->find_item(itemKey) != 0)
                            {
                                char title[21];         // -0x65
                                strncpy(title,
                                        g_scriptStringManager_.findString(
                                            4, "game_server_msg_153", 0),
                                        0x14);
                                char bodyText[256];     // -0x165
                                strncpy(bodyText,
                                        g_scriptStringManager_.findString(
                                            4, "game_server_msg_154", 0),
                                        0xff);
                                unsigned int serverGroup =
                                    (unsigned int)m_member[slot].m_pUser->GetServerGroup();
                                unsigned int characNo =
                                    (unsigned int)reinterpret_cast<CUserCharacInfo*>(
                                        m_member[slot].m_pUser)
                                        ->getCurCharacNo();
                                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                    title, *item, 0, characNo, bodyText,
                                    (int)strlen(bodyText), 8,
                                    (ENUM_SERVER_GROUP)serverGroup, false,
                                    false);
                            }
                        }
                    }
                    else
                    {
                        PacketGuard packet;             // -0x50
                        packet.put_header(0, 0xe);
                        packet.put_byte(item->GetItemSpace());
                        packet.put_short(1);
                        INVEN_TYPE invenType =
                            (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)
                                item->GetItemSpace());
                        reinterpret_cast<CUserCharacInfo*>(
                            m_member[slot].m_pUser)
                            ->getCurCharacInvenR()
                            ->MakeItemPacket(invenType, putSlot, packet);
                        packet.finalize(true);
                        m_member[slot].m_pUser->Send(packet);
                    }
                }

                unsigned int rewardExp =
                    (unsigned int)m_battleField.m_bloodReward.getRewardExp(slot);   // -0x34
                if (1000000 < rewardExp)
                {
                    LogManager::logFormat(
                        1, "party.cpp",
                        __PRETTY_FUNCTION__, 0x3bd4,
                        "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP!! (%u)\n",
                        rewardExp);
                    rewardExp = 1000000;
                }
                int sp = 0;                             // -0x40
                int sfp = 0;                            // -0x44
                if (m_member[slot].m_pUser->gain_exp_sp(
                        (int)rewardExp, sp, sfp, (CUser::eExpAddReason)2, 0,
                        false))
                    m_member[slot].m_pUser->SendNotiPacket((CUser::eSendTarget)2,
                                                   (ENUM_NOTIPACKET)2, 1);
            }
        }
        m_battleField.setBloodState((ENUM_BLOOD_STATE)5);
        SetEPLPState(2);
        int key = gen_timer_key((TIMER_MESSAGE)0x25);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x25, 0x3c, key, 0x25);
        sendInoutConditionDungeon(
            (RetryFailReason::T)checkInoutConditionDungeon());
    }
    else if ((int)msg == 0x25)
    {
        if (GetEPLPState() != 2)
            return 0;
        if (GetSelectedEPLPCmd() == 3)
            return 0;
        finishBloodDungeon();
    }
    else if ((int)msg == 0x22)
    {
        if (m_battleField.getBloodState() < 2)
            return 0;
        if (get_member_count() < 1)
            return 0;
        // P48：ORIG 085b7512/085b7583 对 check_grid_clear/check_end_point/isBloodClear
        // 在 not-all-dead 与 all-dead 两支各独立计算一次（不缓存 clearOk 局部）。
        if (!check_allmember_die())
        {
            if (m_battleField.check_grid_clear() && m_battleField.check_end_point() &&
                m_battleField.m_bloodReward.isBloodClear())
                beginBloodClearProcess(true);
        }
        else
        {
            if (m_battleField.check_grid_clear() && m_battleField.check_end_point() &&
                m_battleField.m_bloodReward.isBloodClear())
                beginBloodClearProcess(true);
            else
                beginBloodClearProcess(false);
        }
    }
    return 1;
}