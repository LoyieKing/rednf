// ============================================================================
// CParty::dungeon_clear —— ORIG 0x85ab736：胜利结算；发放三组奖励物品与金卡，
// 清空奖励槽并注册 0x15 秘商定时器。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x50/0x70 + P167 缺口；325 vs 389
void CParty::dungeon_clear()
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85ab736
    // [G7-sub] 本 .inc 只承载函数头 + SaveCoin 一句（主体在聚合器 cpp，非本泳道写域）。
    // 复核 ORIG 085ab73e/085ab74c：`movl $0x1,0x4(%esp); mov 0x8(%ebp),%eax;
    // mov %eax,(%esp); call SaveCoin` 与我方逐条一致（diff 无该段差异）⇒ 域内无可改点；
    // 余差（我方 395 vs 390 = +5，`movl $0x0,-0x24` 循环初值/声明序）位于 cpp 侧。
    SaveCoin((eCoinSubReason)1);
    // P183：ORIG 对 bd 每个使用点重取址（无引用缓存槽）⇒ 宏直用。
#define bd (m_battleData)
    // [P331/P212 重排声明序] ORIG 序言 `movl $0x0,-0x20(%ebp)` 出现在
    // `lea -0x2c(%ebp); call PacketGuard::C1Ev` **之前** ⇒ 该 0 初始化变量
    // （goldCardCount）声明先于 PacketGuard；memberCount 赋值在 ctor 之后（-0x1c）。
    int goldCardCount = 0;                              // ORIG -0x20（ctor 前）
    PacketGuard packet;                                 // ORIG -0x2c
    int memberCount = get_member_count();               // ORIG -0x1c
    for (int slot = 0; slot <= 3; ++slot)               // -0x18
    {
        if (_checkValidUser(slot))
        {
// §P331 第二形（实测 634→390）：ORIG 对 m_member[slot].m_pUser 与
// m_itemGroups{0,1,2}[slot] 每次访问都从 this 重物化（imul$0x7a 13 处、add$0x79d 4 处、
// add$0x760 3 处；本方原 3/0/0 —— 被 user + g0/g1/g2 四个具名指针吸收）。slot 为循环
// 变量、体内不改 slot/this ⇒ 逐次寻址同物，无可观察行为变化。
#define user (m_member[slot].m_pUser)
#define g0 (m_battleData.m_itemGroups0[slot])
#define g1 (m_battleData.m_itemGroups1[slot])
#define g2 (m_battleData.m_itemGroups2[slot])
            int put = _putItemIntoUser(user, &g0[0], packet);
            put += _putItemIntoUser(user, &g1[0], packet);
            if (HaveAlreadySelectedCard(
                    (char)slot, (eClearRewardCardType_t)1))
            {
                CItem* card = G_CDataManager()->find_item(g2[1].m_addInfo);
                if (card != 0)
                {
                    int price = 0;
                    G_Store()->GetSellItemPrice(g2[1], card,
                                                (short)g2[1].m_addInfo2,
                                                false, price);
                    GetInstanceValueStatistic()->AddValueStatistic(
                        (VALUE_STATISTIC_FIELD)5, user, (unsigned int)price);
                }
                put += _putItemIntoUser(user, &g2[0], packet);
                // [P0/P329] ORIG 0x085abaxx：GetRepeatEvent→IsEventing 结果直测
                //（短路链物化，无 ev40/giveGold 具名槽）。
                if ((GlobalData::s_event_manager->GetRepeatEvent(0x40)->IsEventing(0) ||
                     IsReturnUserParty() || IsEventCharacParty()) &&
                    !IsPremiumGoldCardParty())
                    put += _putItemIntoUser(user, &g2[0], packet);
                ++goldCardCount;
            }
            (void)put;
            g0[0].reset();
            g0[1].reset();
            g1[0].reset();
            g1[1].reset();
            g2[0].reset();
            g2[1].reset();
            bd.ResetEventDungeonClearPoint();
            HistoryLog::WriteDunClear((FILE*)user->m_field796f8);
            OnSuccessedDungeonClear(user);
        }
    }
    // [R3a20 语义修复 2026-09-12] ORIG 0x085abb1b `jne 0x85ab77d`（回边）后 fall-through
    //   0x085abb21/0x085abb2f（call SetEPLPState）… 0x085abc4c（dungeonClear）0x085abc51 jmp epilogue：
    //   收尾块在 for **循环之外**、循环退出（slot>3）即达。旧形置于循环体内 `if (slot > 3)`
    //   ⇒ 4.4 使循环退出边直达 epilogue（我方 .o 0xf99a jmp 0xf9b7），收尾块入口 0xf4d7 恒不可达。
    SetEPLPState(2);
    int sec = 0x3c;                                     // ORIG -0x18
    if (m_secretShop.IsOpen())
        // ORIG dungeon_clear.md 085abb54 `mov 0xa784(%eax)` =
        // SecretShopScript::m_int84（同 TournamentDungeonClear 站点）
        // [R5a15 ④ 真语义修复] ORIG 0x085abb5a-0x085abb68 直证 ×60：
        //   `shl $0x2,%eax; mov %eax,%edx; shl $0x4,%edx; mov %edx,%ecx;
        //    sub %eax,%ecx; mov %ecx,%eax`（x*4 → x*64 → x*60）
        //   旧值 12（我方 0xf895-0xf89b `3x`+`shl $2`）为误抄，改回 0x3c。
        sec = G_CDataManager()->m_secretShop.m_int84 * 0x3c;
    int key = gen_timer_key((TIMER_MESSAGE)0x15);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                GetPartyIndex(), (TIMER_MESSAGE)0x15,
                                sec, key, 0);
    set_state(1);
    sendInoutConditionDungeon(
        (RetryFailReason::T)checkInoutConditionDungeon());
    if (IsPremiumGoldCardParty())
        sendPremiumGoldCardStatistic(m_battleField.m_pDungeon->get_standard_level(),
                                     memberCount, goldCardCount);
    CheckHackAverageHitCount();
    ClearMapHitCount();
    m_secu.dungeonClear();
    return;
}