    // [G7-sub 2026-09-10 复核] 逐段核对 ORIG 0x085bcb40..0x085bd1e7：
    // ① ref/result 两局部 + 字段写序（+0x35 先于 +0x34）✓；② GetTournamentCurRound
    // 两次调用（MakeReward 实参 / expRound，无缓存）✓；③ `goldRate<0→1.0f` 夹取、
    // expRound-1 后 <0 夹取 ✓；④ map find/end const 形（0x085bcdd7/df2 两处 `_ZNKSt3map`）✓；
    // ⑤ 循环内 PacketGuard 每轮构造（0x085bce4e `lea -0x4c`）+ `float deadInit=0.0f`
    //    死槽（0x085bce5e）✓（P232 扩展：float 才发二段 mov）。
    // 余差 2 条全为调用/字符串地址假差（.o 重定位未解析）⇒ 无可改项；p167 仅
    // C1/C2、D1/D2 别名（P93）。
void CParty::MakeTournamentReward(CTournamentDungeonReward& reward)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS get_dungeon_index + 非 const map find/end 3 项 /
    // EXTRA const_iterator 转换构造（P96，改型回归实测）+ memcpy。@0x85bcb40
    bool victory = m_tournamentVictory;                 // +0x11a
    // ORIG 0x085bcb67..0x085bcba0：round 无槽（eax 直传，P0）。
    reward.MakeReward(&m_battleField.m_pDungeon->m_map8ac, &m_battleField.m_pDungeon->m_itemRate8c4,
                      m_battleField.GetTournamentCurRound(), victory);

    // ORIG 0x085bcba5..0x085bcbcc：G_CDataManager 结果具名落槽（-0x30），实参内联
    // m_battleField.get_dungeon_index()（P0：无 dungIdx 局部）。
    // const 视图：ORIG 0x085bcdd7 find / 0x085bcdf2 end 均为 `_ZNKSt3map...`（P323）。
    CDataManager* dm = G_CDataManager();
    CDungeon const* dungeon = dm->find_dungeon(m_battleField.get_dungeon_index());
    int memberCount = get_member_count();               // -0x2c
    if (dungeon == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x4540, "Unknown Dungeon %d", m_battleField.get_dungeon_index());
        return;
    }
    if (memberCount < 1)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x4545, "No Member[%d] in party", memberCount);
        return;
    }

    int stdLevel = dungeon->get_standard_level();       // -0x28
    int refLevel = stdLevel;                            // -0x24（ORIG local_24 副本）
    WongWork::stGenerateRefData_t ref;                  // -0xdc
    // ORIG 0x085bcc98/0x085bcca1 顺序：+0x35（diff）先于 +0x34（level）。
    ref.m_field35 = (char)m_battleField.get_dungeon_diff();
    ref.m_field34 = (char)refLevel;
    ref.m_rateSlot = (char)memberCount;
    // ORIG 0x085bccb0..0x085bccec 逐条：movl $1→+0x38、this+0x374→+0x3c、
    // this+0x36c→+0x3e、movw $0→+0x40、this+0x370→+0x42（均无中转槽）。
    ref.m_rankBonusIndex = 1;                                   // +0x38
    ref.m_goldBase = (unsigned short)m_battleData.m_field48;    // +0x3c（this+0x374）
    ref.m_goldFactor = (unsigned short)m_battleData.m_killedMonster1;  // +0x3e（this+0x36c）
    ref.m_field40 = 0;                                          // +0x40
    ref.m_field42 = (unsigned short)m_battleData.m_killedMonster2;     // +0x42（this+0x370）

    WongWork::stGenerateResult_t result;                // -0x94
    // ORIG 0x085bcd01..0x085bcd3c：全链内联（无 genMgr 槽），虚槽 +0x0c
    // = CItemGenerator::generateSpecificItem（CItemGenerator.h vtable 注释）。
    G_CDataManager()->getItemGenerator()
        ->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
        ->generateSpecificItem(ref, result);

    float goldRate = dungeon->m_tournamentGoldRate;
    if (goldRate < 0.0f)
        goldRate = 1.0f;
    reward.SetRewardGold(result, goldRate);

    int exp = 0;                                        // -0x18
    int expRound = m_battleField.GetTournamentCurRound();   // -0x14
    if (!victory)
        expRound = expRound - 1;
    if (expRound < 0)
        expRound = 0;

    // b28d-49 判词：N 封顶。P167 差集 ORIG 为非 const find + const_iterator 转换构造
    //（EXTRA），但同批 iterator 改型实测全 TU identical 回归 ⇒ 保持 const_iterator。
    std::map<unsigned char, unsigned int>::const_iterator itExp =
        dungeon->m_map8d8.find((unsigned char)expRound);
    if (itExp != dungeon->m_map8d8.end())
        exp = (int)itExp->second;

    for (int slot = 0; slot < 4; ++slot)                // -0x10
    {
        PacketGuard packet;                             // -0x4c
        packet.clear();
        packet.put_header(0, 0x184);
        float deadInit = 0.0f;                          // ORIG 0x085bce5e 死存储（local_10=0）
        (void)deadInit;
        // ORIG 0x085bce78：负极性早退（call; xor $1; test %al; jne <dtor>）。
        if (!_checkValidUser(slot))
            continue;
        // P183/P0：ORIG 每处重新展开 m_member[slot].m_pUser 链（6 条），无 user 缓存。
        m_member[slot].m_pUser->setChattingMessageCount(0);
        reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)->calcHelpAbuseRatio(
            (unsigned int)exp, isHelpAbuseParty(m_member[slot].m_pUser));
        reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)->reset_level_before_dungeon();

        int sp = 0;                                 // -0x50
        int sfp = 0;                                // -0x54
        if (m_member[slot].m_pUser->gain_exp_sp(exp, sp, sfp,
                              (CUser::eExpAddReason)2, 0, false))
        {
            PacketGuard infoPacket;                 // -0x60
            infoPacket.clear();
            infoPacket.put_header(0, 2);
            infoPacket.put_byte(1);
            infoPacket.put_short(1);
            m_member[slot].m_pUser->make_basic_info((char*)&infoPacket, 1);
            infoPacket.finalize(true);
            m_member[slot].m_pUser->Send(infoPacket);
        }
        packet.put_byte(expRound);
        packet.put_int(exp);
        // ORIG 0x085bd085：`cmpb $0,-0x31; je <put_byte(0)>` ⇒ 正极性 then=MakeQuestRewardPacket。
        if (victory)
            MakeQuestRewardPacket(&packet, m_member[slot].m_pUser);
        else
            packet.put_byte(0);
        reward.MakeRewardPacket(&packet, victory);
        packet.finalize(true);
        m_member[slot].m_pUser->Send(packet);
    }
    SetTournamentDungeonClearState(1);
    InsertTournamentDungeonTimer((TIMER_MESSAGE)0x2e);
