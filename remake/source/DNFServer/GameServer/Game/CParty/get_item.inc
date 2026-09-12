// ---- CParty::get_item（ORIG T 0x85a3b98）----
// §6.2 巨件核销：P167 264/264 [0缺0多]；gap 2499→2；残差归因 N82（ORIG push %edi+槽基
// 错位）+ N18b/X1a（`!=1` 比较 4.4 恒折 cmp$1，ORIG xor$1+test 共 12 处不可翻）。
// 真修：①金币循环非 winner 臂原多发一次 Inven_Item::get_add_info()（EXTRA +1）——
// ORIG 85a410f 循环外把 share 拷入 -0xb4 副本槽、三目 else 臂读副本，补 noBonusGain 局部；
// ②删 `CBattle_Field& bf` 缓存槽（P183），9 处调用展开为 m_battleField 直调，
// 复现 ORIG 每次 lea 0xb24(%eax) 重物化。
// 返回 1=拾取成功；0=失败（错误码经 1/0x2e 包下发）。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x32c/0x350；2497 vs 2494
bool CParty::get_item(CUser* user, int itemIdx, bool flag, char c, int& outIdx,
                     unsigned char& outUpgrade, bool& outStackable)
{
    // b28d-49 判词：N 封顶（N82 帧/槽 + N84 rep movsl）。P167 CLEAR（264/264）。@0x85a3b98
    // [轴 B 自校·实测] axisB_points.tsv 三行（0x85a41e7/0x85a4444/0x85a4ab8）逐点
    // 按地址自校：①0x85a41e7 `cmpl $0x7a11f,-0xb8(%ebp); jle` 与我方
    // `cmpl $0x7a11f,...; jle` 同为有符号 ⇒ 一致（表内假阳性）；②0x85a4444 位于
    // ORIG 的 switch 决策树 `cmp $0x6802,%eax; je; cmp $0x6802; jg`（**有符号**），
    // 我方 `switch (pet1.m_addInfo)` 的 m_addInfo 为 unsigned int ⇒ 4.4 折 `ja`
    // ⇒ **真分叉**（同型第二处 switch 亦然）⇒ 两处改 `switch ((int)…)` 后复编
    // 实测：`ja`→`jg`（3 个决策点全对齐 ORIG），diff 段 cc-fork 对 26→23、
    // 四数 146/327/1/33 逐值不变，保留；③0x85a4ab8 `cmpl $0x3; setle` 循环尾
    // 与我方 setle 一致 ⇒ 假阳性。
    char routingEligible = 1;                       // -0xe9
    CUser* winner = 0;                              // -0xe8
    int weights[4] = { 0, 0, 0, 0 };                // -0x234
    int sellPrice = 0;                              // -0x244
    int typeMark = 0;                               // -0xe4

    PacketGuard packet;                             // -0x240

    int slotNo = (int)GetMemberSlotNo(user);        // -0xe0
    if (slotNo != -1 && m_battleData.m_arr54[slotNo] != 1)
    {
        // [G7-sub] P0 槽律：ORIG 085a3c71 `call GetCreatureMgrR` 结果**无槽**
        // （直进 (%esp) → GetCreatureItemId），085a3c9c `call getHackAnalyzer`
        // 结果亦无槽（直进 (%esp) → addServerHackCnt）⇒ 原码为表达式链，
        // 无 cm/hack 具名指针；我方原 -0x100/-0xfc 两槽已删。
        if (user->getCurCharacInvenR()->GetCreatureMgrR()->GetCreatureItemId() !=
            (int)0xf631)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x132, 1, 0, 0);
        }
    }

    if (m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->IsEnterEachMap())
        m_battleField.SetCurSeatNo(slotNo);

    map_item mi;                                    // -0x2bc
    char found = m_battleField.get_field_item(itemIdx, mi);    // -0xd9

    if (found == 0)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        if ((char)c > 3)
            packet.put_byte(0x51);
        else
            packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);

        // [R12a10 ④] ORIG 0x085a3dd2..0x085a3df9：`IsAutoCreated() || get_member_count()==1`
        // 成立即进入告警块（cond 物化：0x085a3dd9 `jne +0x253`(mov $1) /
        // 0x085a3de9 `cmp $1,%eax; jne +0x25a`(mov $0) / 0x085a3df9 `test %al,%al;
        // je +0x327` 跳过整块）。
        // 旧写 `!IsAutoCreated() && get_member_count() != 1` 使上报条件完全相反
        // （ORIG 自动队伍/单人时上报 0x12d，我方反之）。
        if (IsAutoCreated() || get_member_count() == 1)
        {
            // ORIG 0x085a3dff `movb $0x0,-0xc9(%ebp)` 预清 + 0x085a3e41
            // `movb $0x1,-0xc9(%ebp)` 条件置位（P198① 两句式，禁初始化式合并）。
            bool visited = false;   // -0xc9
            if (!m_battleField.check_start_point() || m_battleField.IsVisitedCurrentMap())
                visited = true;
            if (!visited || !m_battleField.is_last_field_item(itemIdx))
            {
                // [G7-sub] P0 槽律：ORIG 085a40xx 同形（getHackAnalyzer 结果无槽，
                // 直进 (%esp) → addServerHackCnt）⇒ 删第二处 hack 具名。
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x12d, 1, 0, 0);
            }
        }
        return 0;
    }

    int addInfo = mi.m_item.m_addInfo;
    if (get_member_count() < 1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    if (addInfo == 0 || addInfo == 0x4af)
    {
        bool goldGen = (addInfo == 0x4af);
        if (!goldGen)
        {
            int liveCount = get_live_count_enter_map();                  // -0xc8
            if (liveCount < 1)
            {
                packet.put_header(1, 0x2e);
                packet.put_byte(0);
                packet.put_byte(0x15);
                packet.finalize(true);
                user->Send(packet);
                return 0;
            }

            int base;                                    // 金币基数
            int money = mi.m_item.get_add_info();
            if (money > 499999)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x165c, 4);
                trace("Dungeon(%d) drop money(%d) on %d' party member with difficulty %d",
                      // [V331b10 核验修复 2026-09-12] ORIG 0x085a4058 `mov 0xcb8(%eax),%edi`
                      // 为**单层**读 this+0xcb8 = CBattle_Field::m_dungeonIndex（battleField
                      // @0xb24 + 0x194，CBattle_Field.h:485-488 布局直证），非
                      // `m_pDungeon->m_index`（两层解引用，ORIG 无 0xcac/0x8 组合）。
                      m_battleField.m_dungeonIndex, money, get_member_count(),
                      m_battleField.get_dungeon_diff());
            }
            int share = mi.m_item.get_add_info() / liveCount;   // -0xc0
            int rem = mi.m_item.get_add_info() % liveCount;     // -0xbc
            base = share + rem;                                 // -0xb8
            int noBonusGain = share;                            // -0xb4（ORIG 85a410f：非 winner 臂读 share 副本槽，循环内不再调 get_add_info）

            packet.put_header(0, 0x27);
            packet.put_short(itemIdx);
            packet.put_short(user->get_unique_id() & 0xffff);

            for (int i = 0; i <= 3; ++i)
            {
                // [V331b10 核验 2026-09-12] ORIG 0x085a4c62 `movl $0x0,-0x74(%ebp)`（循环体首，
                // 槽 -0x74 ↔ 我方 -0x88）⇒ 原码该局部为**带初始化声明**（P348：ORIG 有
                // `movl $0x0` 必须保留）。查 ORIG 全引用表：首清与紧随的写（0x085a4d38）之间
                // 无读 ⇒ 该 store 语义为 dead，但按 P348 照抄形态。
                int gain = 0;
                if (!_checkValidUser(i))
                {
                    packet.put_byte(0);
                    packet.put_int(0);
                    continue;
                }
                packet.put_byte(1);

                gain = (m_member[i].m_pUser == user) ? base : noBonusGain;
                if (base > 499999)
                {
                    cMyTrace trace(
                        __PRETTY_FUNCTION__,
                        0x1688, 4);
                    trace("User(%s) Charac(%s) gain money(%d), old money is %d.",
                          NumberToString(m_member[i].m_pUser->get_acc_id(), 0),
                          m_member[i].m_pUser->getCurCharacName(), base,
                          m_member[i].m_pUser->getCurCharacInvenR()->get_money());
                }
                if (m_battleData.m_arr58[i] != 1)
                    gain = 0;

                const int gainBase = gain;             // -0xa4（ORIG：bonusGold/guildFund 均基于初始 gain）
                int bonusGold = 0;                   // 展示加成合计（local_a4）
                int guildFund = 0;                   // local_9c
                int manageAmt = 0;                   // local_a0：gain_money 第4参
                if (mi.m_count != 0)
                {
                    bonusGold =
                        m_member[i].m_pUser->getCurCharacInvenR()->GetCreatureMgrR()
                            ->GetExtraGold(gainBase);
                    gain += bonusGold;
                }
                if (mi.m_count != 0)
                {
                    const int over =
                        m_member[i].m_pUser->getCurCharacInvenR()->GetCreatureMgrR()
                            ->GetOverSkillExtraGold(gainBase);
                    bonusGold += over;
                    gain += over;
                }
                // [R17a10 ④] ORIG 守卫为**两个合取项**：0x085a43b9 `movzbl -0x2bc`
                //   (=mi.m_count) + 0x085a43c8 `cmpl $0x0,-0xa4(%ebp)`(=gainBase)
                //   两跳同落 +0x99f ⇒ 源码 `mi.m_count != 0 && gainBase > 0`。
                //   旧写仅 `mi.m_count != 0`：gainBase<=0 时我方仍执行本块
                //   （多发 GetInvenSlot(0,11)/加成包）。
                if (mi.m_count != 0 && gainBase > 0)
                {
                    Inven_Item pet1 = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 11);
                    int pct = 0;
                    // [b28f-CTS 轴B] ORIG 0x085a4444/0x085a444d/0x085a4473 处
                    //   `cmp $0x6802,%eax; jg`（**有符号**序）；`m_addInfo` 为
                    //   unsigned int ⇒ 4.4 折无符号 `ja`。
                    switch ((int)pet1.m_addInfo)
                    {
                    case 0x6603: case 0x66e5: case 0x6802: case 0x28e4a3: case 0x28e4a6:
                        pct = 3; break;
                    case 0x6622: case 0x66e6: case 0x6803: case 0x68a2:
                        pct = 4; break;
                    case 0x28e4a8:
                        pct = 5; break;
                    }
                    int extra = (gainBase * pct) / 100;
                    bonusGold += extra;
                    gain += extra;

                    Inven_Item pet2 = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 19);
                    if (pet2.m_addInfo == 0x5e7f)
                    {
                        // [R17a10 ④] ORIG 0x085a45b2 `fildl v; fldl 0x8cbbbe0; fmulp`
                        // = (double)v * 0.1（0x8cbbbe0 raw = 9a 99 99 99 99 99 b9 3f = 0.1）
                        // 旧写 2.5 使该分支额外金币/gain 放大 25 倍（真语义 bug）。
                        int extra = (int)(0.1 * (double)gainBase);
                        bonusGold += extra;
                        gain += extra;
                    }
                }
                if (mi.m_count != 0)
                {
                    // ORIG get_item.md：CalcurateManageBonus 基准为当前累计 gain
                    //（local_ac/local_78，已含生物/宠物加成），非初始 gainBase。
                    float mb = CalcurateManageBonus((ManageBonusType::T)1,
                                                    m_member[i].m_pUser,
                                                    (unsigned int)gain);
                    int manage = (int)(long double)mb;
                    bonusGold += manage;
                    gain += manage;
                    manageAmt = manage;   // gain_money 第4参（local_a0/local_64）
                    GetInstanceCirculationStatistic()->AddCirculationStatistic(
                        (CIRCULATION_STATISTIC_FIELD)0x11, m_member[i].m_pUser,
                        (unsigned int)manage);
                }
                if (mi.m_count != 0 &&
                    m_member[i].m_pUser->get_charac_guildkey() != 0 &&
                    m_member[i].m_pUser->IsCompleteGuildFund() == 0)
                {
                    bool fixedBand =
                        (G_CDataManager()->m_guildParameter.m_field104 <= gainBase) &&
                        (G_CDataManager()->m_guildParameter.m_field108 >= gain);
                    if (fixedBand)
                    {
                        guildFund = G_CDataManager()->m_guildParameter.m_field10c;
                    }
                    else if (G_CDataManager()->m_guildParameter.m_field108 < gainBase)
                    {
                        guildFund = (int)((long double)G_CDataManager()->m_guildParameter.m_field110 * (double)gainBase);
                    }
                    if (guildFund <= gain && guildFund > 0)
                    {
                        gain -= guildFund;
                        GetInstanceCirculationStatistic()->AddCirculationStatistic(
                            (CIRCULATION_STATISTIC_FIELD)0x2a, m_member[i].m_pUser,
                            (unsigned int)guildFund);
                    }
                    if (guildFund > 0)
                    {
                        CGuildServerProxy* proxy =
                            GlobalData::s_guild_proxy_mgr->GetServerProxy(
                                m_member[i].m_pUser->GetServerGroup());
                        proxy->SendAddGuildFund(
                            m_member[i].m_pUser->get_charac_no(-1),
                            m_member[i].m_pUser->get_charac_guildkey(), guildFund,
                            (Packet_Add_Guild_Fund::eFundType)0);
                    }
                }

                CInventory* invenW = m_member[i].m_pUser->getCurCharacInvenW();
                int gained = invenW->gain_money(
                    gain, (eMoneyAddReason)4, true, manageAmt);
                if (mi.m_count != 0)
                    m_member[i].m_pUser->saveTaxMoneyForUpperMember(gain);
                if (gained < gain)
                {
                    if (gained < 0)
                        gained = 0;
                    m_member[i].m_pUser->SendMoneyFullReason(
                        (ENUM_MONEY_FULL_REASON)0, (unsigned long)gain,
                        (unsigned long)gained);
                }
                // ORIG 0x085a4a01 区（money 分支）：statistic 实参 = gained（-0x94，与 put_int 同值）
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)1, m_member[i].m_pUser,
                    (unsigned int)gained);

                packet.put_int(gained);
                packet.put_byte(1);
                packet.put_int(bonusGold);
                packet.put_int(guildFund);
            }
            // ORIG 0x085a4ad9 区：money 分支自带尾块（finalize+send_to_party+pickup_item+0x2e 错误包）
            packet.finalize(true);
            send_to_party(packet);
            int picked = 0;
            picked = m_battleField.pickup_item(itemIdx);
            if (picked >= 1)
            {
                packet.put_header(1, 0x2e);
                packet.put_byte(0);
                packet.put_byte(picked);
                packet.finalize(true);
                send_to_party(packet);
                user->Send(packet);
                return 0;
            }
            if (mi.m_count != 0)
            {
                outIdx = mi.m_item.m_addInfo;
                outUpgrade = 0;
            }
            else
            {
                outIdx = -1;
                outUpgrade = 0;
            }
            return 1;
        }
        else
        {
            packet.put_header(0, 0x27);
            packet.put_short(itemIdx);
            packet.put_short(user->get_unique_id() & 0xffff);

            for (int i = 0; i <= 3; ++i)
            {
                // [V331b10 核验 2026-09-12] 同前（第二个分支循环）：ORIG 0x085a4e27
                // `movl $0x0,-0x74(%ebp)` ⇒ 原码带初始化声明（P348）。
                int gain = 0;
                if (!_checkValidUser(i))
                {
                    packet.put_byte(0);
                    packet.put_int(0);
                    continue;
                }
                packet.put_byte(1);

                WongWork::CItemGeneratorMgr* mgr = G_CDataManager()->getItemGenerator();
                WongWork::CItemGenerator* gen =
                    mgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2);
                unsigned char lv = (unsigned char)m_member[i].m_pUser->get_charac_level();
                int* table = gen->getGeneratorCommonData()->getGoldGenTable(lv);
                int rnd = GetRandomRange(-table[1], table[1]);
                gain = table[0] + (table[0] * rnd) / 100;
                if (gain > 499999)
                {
                    cMyTrace trace(
                        __PRETTY_FUNCTION__,
                        0x1789, 4);
                    trace("User(%s) Charac(%s) gain money(%d), old money is %d.",
                          NumberToString(m_member[i].m_pUser->get_acc_id(), 0),
                          m_member[i].m_pUser->getCurCharacName(), gain,
                          m_member[i].m_pUser->getCurCharacInvenR()->get_money());
                }
                if (m_battleData.m_arr58[i] != 1)
                    gain = 0;

                const int gainBase = gain;             // -0x68（ORIG：bonusGold/guildFund 均基于初始 gain）
                int bonusGold = 0;                   // 展示加成合计（local_68）
                int guildFund = 0;                   // local_60
                int manageAmt = 0;                   // local_64：gain_money 第4参
                if (mi.m_count != 0)
                {
                    bonusGold =
                        m_member[i].m_pUser->getCurCharacInvenR()->GetCreatureMgrR()
                            ->GetExtraGold(gainBase);
                    gain += bonusGold;
                }
                if (mi.m_count != 0)
                {
                    const int over =
                        m_member[i].m_pUser->getCurCharacInvenR()->GetCreatureMgrR()
                            ->GetOverSkillExtraGold(gainBase);
                    bonusGold += over;
                    gain += over;
                }
                // [R17a10 ④] 第二处同形合取项（ORIG 0x085a4537 `movzbl -0x2bc`;
                //   0x085a4546 `cmpl $0x0,-0xa4(%ebp)`; 两跳同落 +0xa59）。
                if (mi.m_count != 0 && gainBase > 0)
                {
                    Inven_Item pet1 = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 11);
                    int pct = 0;
                    // [b28f-CTS 轴B] 同上前一块：ORIG 0x085a4f6a 区 `jg`（**有符号**序，
                    //   `m_addInfo` 为 unsigned int）⇒ 显式 (int) 收窄复现 ORIG 的 jg 树。
                    // [R10a10 ④#12] ORIG 第二处决策树 0x85a4f45..0x85a4fa5 **仅 7 个 case**
                    //   （cmp 链 1189-1206 无 0x28e4a3/0x28e4a6/0x28e4a8；第一处
                    //   0x85a4438..0x85a448f 两侧一致含之）⇒ 删这三 case（我方原多写）。
                    switch ((int)pet1.m_addInfo)
                    {
                    case 0x6603: case 0x66e5: case 0x6802:
                        pct = 3; break;
                    case 0x6622: case 0x66e6: case 0x6803: case 0x68a2:
                        pct = 4; break;
                    }
                    int extra = (gainBase * pct) / 100;
                    bonusGold += extra;
                    gain += extra;

                    Inven_Item pet2 = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 19);
                    if (pet2.m_addInfo == 0x5e7f)
                    {
                        // [R17a10 ④] 同第一处：ORIG 0x085a504d `fldl 0x8cbbbe0`(=0.1)，
                        // 非 2.5。
                        int extra = (int)(0.1 * (double)gainBase);
                        bonusGold += extra;
                        gain += extra;
                    }
                }
                if (mi.m_count != 0)
                {
                    // ORIG get_item.md：CalcurateManageBonus 基准为当前累计 gain
                    //（local_ac/local_78，已含生物/宠物加成），非初始 gainBase。
                    float mb = CalcurateManageBonus((ManageBonusType::T)1,
                                                    m_member[i].m_pUser,
                                                    (unsigned int)gain);
                    int manage = (int)(long double)mb;
                    bonusGold += manage;
                    gain += manage;
                    manageAmt = manage;   // gain_money 第4参（local_a0/local_64）
                    GetInstanceCirculationStatistic()->AddCirculationStatistic(
                        (CIRCULATION_STATISTIC_FIELD)0x11, m_member[i].m_pUser,
                        (unsigned int)manage);
                }
                if (mi.m_count != 0 &&
                    m_member[i].m_pUser->get_charac_guildkey() != 0 &&
                    m_member[i].m_pUser->IsCompleteGuildFund() == 0)
                {
                    bool fixedBand =
                        (G_CDataManager()->m_guildParameter.m_field104 <= gainBase) &&
                        (G_CDataManager()->m_guildParameter.m_field108 >= gainBase);
                    if (fixedBand)
                    {
                        guildFund = G_CDataManager()->m_guildParameter.m_field10c;
                    }
                    else if (G_CDataManager()->m_guildParameter.m_field108 < gainBase)
                    {
                        guildFund = (int)((long double)G_CDataManager()->m_guildParameter.m_field110 * (double)gainBase);
                    }
                    if (guildFund <= gain && guildFund > 0)
                    {
                        gain -= guildFund;
                        GetInstanceCirculationStatistic()->AddCirculationStatistic(
                            (CIRCULATION_STATISTIC_FIELD)0x2a, m_member[i].m_pUser,
                            (unsigned int)guildFund);
                    }
                    if (guildFund > 0)
                    {
                        CGuildServerProxy* proxy =
                            GlobalData::s_guild_proxy_mgr->GetServerProxy(
                                m_member[i].m_pUser->GetServerGroup());
                        proxy->SendAddGuildFund(
                            m_member[i].m_pUser->get_charac_no(-1),
                            m_member[i].m_pUser->get_charac_guildkey(), guildFund,
                            (Packet_Add_Guild_Fund::eFundType)0);
                    }
                }

                CInventory* invenW = m_member[i].m_pUser->getCurCharacInvenW();
                int gained = invenW->gain_money(
                    gain, (eMoneyAddReason)0x1a, true, manageAmt);
                if (mi.m_count != 0)
                    m_member[i].m_pUser->saveTaxMoneyForUpperMember(gain);
                if (gained < gain)
                {
                    if (gained < 0)
                        gained = 0;
                    m_member[i].m_pUser->SendMoneyFullReason(
                        (ENUM_MONEY_FULL_REASON)0, (unsigned long)gain,
                        (unsigned long)gained);
                }
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)1, m_member[i].m_pUser,
                    (unsigned int)gain);

                packet.put_int(gained);
                packet.put_byte(1);
                packet.put_int(bonusGold);
                packet.put_int(guildFund);
            }
            // ORIG 0x085a54bf 区：0x4af 分支自带尾块（与 money 分支同构的第二份拷贝）
            packet.finalize(true);
            send_to_party(packet);
            int picked = 0;
            picked = m_battleField.pickup_item(itemIdx);
            if (picked >= 1)
            {
                packet.put_header(1, 0x2e);
                packet.put_byte(0);
                packet.put_byte(picked);
                packet.finalize(true);
                send_to_party(packet);
                user->Send(packet);
                return 0;
            }
            if (mi.m_count != 0)
            {
                outIdx = mi.m_item.m_addInfo;
                outUpgrade = 0;
            }
            else
            {
                outIdx = -1;
                outUpgrade = 0;
            }
            return 1;
        }
    }

    CItem* item = G_CDataManager()->find_item(addInfo);   // -0x40
    if (item == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x1878, "G_CDataManager()->find_item(%d) fail", addInfo);
        return 0;
    }

    if (item->is_stackable())
    {
        outStackable = true;
        if (item->GetItemType() == 9)
            typeMark = 7;
    }
    else
    {
        if (item->IsCreatureItem())
            typeMark = 7;
        else if (item->IsAvatarItem())
            typeMark = 1;
    }

    // ORIG 0x085a563b 区：史诗路由物品先经 CSpecialItemRoutingManager::StartRouting
    // 起路由；成功(返回 0)即消费返回 1，失败(返回码≠0)发 0x2e 错误包返回 0。
    // （Ghidra C 未渲染此块，按汇编 + CBossStage::RoutingPickupItem 权威模板转写。）
    // [R17 ④ 热补复刻] ORIG 0x085a56ce `cmp $0x4,%eax`（raw 83 f8 04）+ 0x085a56d1
    // raw `eb 16`（**无条件** jmp 0x085a56e9；原字节应为 `74 16`=je）⇒ 比较后恒跳
    // `mov $0x0,%eax`（判定 b 恒 0）；IsRoutingItem 调用点 0x085a56d9 与主线
    // 0x085a56f6..0x085a57c8（含 StartRouting 0x085a572e）**恒不可达**
    // （Ghidra C 同证：`/* WARNING: Removing unreachable block (ram,0x085a56f6) */` 等三处）。
    // 依 P337：恒 0 `volatile bool` 守卫强制发射该段并与 ORIG 同形；不删块结构。
    char routingChecked = 0;                 // 判定 b：热补后恒 0（ORIG `mov $0x0,%eax`）
    volatile bool routingHotfix = false;     // ORIG 0x085a56d1 热补等效：恒 0，读不可折叠
    if (item->get_rarity() != 4)             // ORIG 0x085a56c9 call + 0x085a56ce cmp $4
    {
        if (routingHotfix)                   // 恒假 ⇒ 下列调用与主线恒不可达
            routingChecked = item->IsRoutingItem();   // ORIG 0x085a56d9（热补后无入口）
    }
    if (routingChecked)
    {
        CSpecialItemRoutingManager* mgr =
            G_CGameManager()->GetSpecialItemRoutingManager();
        int ret = 0;
        ret = mgr->StartRouting(user, mi.m_item, (unsigned int)itemIdx);
        if (ret == 0)
            return 1;
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    if (get_member_count() > 1)
    {
        // ORIG 0x085a57e2：movzbl -0x2bc;xor $0x1;test;je → map_item 首字节 != 1 时清资格
        if (mi.m_count != 1)
            routingEligible = 0;
        if (item->get_rarity() > 3)
            routingEligible = (char)flag;
        if (mi.m_count != 0 && item->isEpicRoutingItem())
            routingEligible = (char)flag;

        if (routingEligible != 0)
        {
            if (item->check_item_routing() || item->IsAvatarItem())
            {
                int diceNext = m_routingDice + 1;                 // -0x34
                if (diceNext >= get_member_count())
                    diceNext = 0;
                int seq = 0;                                   // -0x30
                int ownerBias = 0;                             // -0x2c
                if (mi.m_ownerId != 0)
                {
                    ownerBias = get_rand_int(0xb) + 0x59;
                }
                for (int i = 0; i <= 3; ++i)
                {
                    if (!_checkValidUser(i))
                        continue;
                    // ORIG 0x085a591a：movzbl 0x380(...);xor $0x1;test;jne skip → 仅 liveFlag==1 参与
                    if (m_battleData.m_arr54[i] != 1)
                        continue;
                    const CInventory* invR = m_member[i].m_pUser->getCurCharacInvenR();
                    int w = invR->tryInsertItemIntoInventory(mi.m_item);
                    if (w == -1)
                        continue;
                    weights[i] += 1;
                    if (seq == diceNext)
                    {
                        weights[i] += 0x28;
                        m_routingDice = seq;
                    }
                    bool jobBonus;
                    if (item->is_stackable())
                        jobBonus = false;
                    else if (item->GetItemType() != 10)
                        jobBonus = false;
                    else
                        jobBonus = item->check_job_type(
                            m_member[i].m_pUser->get_charac_job()) != 0;
                    if (jobBonus)
                        weights[i] += 0x1e;
                    weights[i] += get_rand_int(100 - weights[i]);
                    if (mi.m_ownerId != 0)
                    {
                        if (m_member[i].m_pUser == (CUser*)mi.m_ownerId)
                            weights[i] = ownerBias;
                        else if (weights[i] >= ownerBias)
                            weights[i] = ownerBias - 1;
                    }
                    ++seq;
                }
    
                int best = 0;                                  // -0x28
                for (int i = 0; i <= 3; ++i)
                {
                    if (!_checkValidUser(i))
                        continue;
                    // ORIG 0x085a5bad：同上，仅 liveFlag==1 参与最优选择
                    if (m_battleData.m_arr54[i] != 1)
                        continue;
                    if (weights[i] > best)
                    {
                        best = weights[i];
                        winner = m_member[i].m_pUser;
                    }
                    else if (weights[i] == best && weights[i] > 0)
                    {
                        weights[i] -= 1;
                    }
                }
            }
            else
            {
                winner = user;
            }
        }
        else
        {
            winner = user;
        }
    }
    else
    {
        winner = user;
    }

    if (winner == 0)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x16);
        packet.finalize(true);
        send_to_party(packet);
        return 0;
    }

    G_Store()->GetSellItemPrice(mi.m_item, item, (short)mi.m_item.m_fieldb,
                                false, sellPrice);

    if (item->IsRandomOption() && !winner->getCheckPickUpRandomOptionItem())
    {
        winner->setCheckPickUpRandomOptionItem(true);
        DB_InsertRandomOptionItemInform::makeRequest(winner->GetUID(),
                                                     winner->get_acc_id(), 1);
        PacketGuard optPacket;
        optPacket.clear();
        optPacket.put_header(0, 0x171);
        optPacket.put_short(winner->get_unique_id() & 0xffff);
        optPacket.put_byte(1);
        optPacket.finalize(true);
        winner->Send(optPacket);
    }
    if (item->IsRandomOption())
    {
        statistc_proxy::add(
            winner, "log_random_option", "pick_cnt", 1u, "%d,'%s'",
            (int)winner->get_charac_10_level_section(),
            GlobalData::s_systemTime_.getCurDate());
    }

    int slot;                                            // -0xd4
    if (!mi.m_item.isAvatarItemType())
    {
        slot = winner->getCurCharacInvenW()->insertItemIntoInventory(
            mi.m_item, (eItemAddReason)4, true, true);
    }
    else
    {
        char ipg[24];                                    // -0x268
        WongWork::CGenUniqueNo::genIPGNo((WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                                         winner->getCurCharacNo(), ipg);
        slot = winner->addAvatarItem(mi.m_item.m_addInfo, 0,
                                     (char)mi.m_item.m_fieldb, ipg,
                                     (eAvatarItemAddReason)0xf);
    }

    if (slot == -1)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)2,
                                                   winner,
                                                   (unsigned int)sellPrice);
    int picked = m_battleField.pickup_item(itemIdx);                // -0xd0
    if (picked >= 1)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x19bb, 5);
        trace("m_field_data.pickup_item( %d/%d ) ", itemIdx, addInfo);
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(picked);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    _onGetItem(winner, (unsigned int)addInfo, (unsigned int)mi.m_item.m_addInfo2);

    packet.put_header(0, 0x27);
    packet.put_short(itemIdx);
    packet.put_short(user->get_unique_id() & 0xffff);
    for (int i = 0; i <= 3; ++i)
        packet.put_byte(weights[i]);
    packet.put_short(winner->get_unique_id() & 0xffff);
    packet.put_short(slot);
    packet.put_byte(typeMark);
    packet.finalize(true);
    send_to_party(packet);

    if (mi.m_count != 0)
    {
        outIdx = mi.m_item.m_addInfo;
        outUpgrade = mi.m_item.GetUpgrade();
    }
    else
    {
        outIdx = -1;
        outUpgrade = 0;
    }

    if (mi.m_count != 0 && mi.m_item.m_field1 == 2)
    {
        winner->SendAradEventItemLog((unsigned long)mi.m_item.m_addInfo,
                                     mi.m_item.get_add_info());
    }
    return 1;
}