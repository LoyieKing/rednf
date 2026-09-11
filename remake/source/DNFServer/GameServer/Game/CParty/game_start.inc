// ---- CParty::game_start（ORIG T 0x859d718）----
// 状态==2 直接返回；否则逐成员 make_basic_info/goto_dungeon/交易清理，
// 广播 0x2(基本)/0x3(进图名单)/0x1b(血宫信息) 三包；村庄/区域不一致走
// HACK_VILL_AREA 日志并 DisConnSig(0x18) 全员断线。
// [SweepCParty] 补 GetPartyTelePort()（旧码直取 &m_pad1ad0）+ hellQuests 迭代器遍历
// （旧码下标形产 EXTRA operator[]）；p167 104=104 CLEAR（余 C1/C2、D1/D2 别名）。
void CParty::game_start(CUser* user)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS vector<int> begin/end/de/pp/ne + GetPartyTelePort
    // 6 项 / EXTRA operator[]（P96 迭代器形，改型回归实测）。@0x859d718
    if (m_state == 2)
        return;

    PacketGuard basicPacket;                                // -0x54
    basicPacket.clear();
    basicPacket.put_header(0, 2);
    basicPacket.put_byte(1);
    basicPacket.put_short(get_member_count());

    m_tournamentVictory = 0;                                // +0x11a
    m_routingDice = get_rand_int(get_member_count());          // +0x144 路由起始槽
    m_field185c = GlobalData::s_systemTime_.getCurSec();    // +0x185c

    PacketGuard enterPacket;                                // -0x60
    enterPacket.put_header(0, 3);
    int liveCount = 0;                                      // -0x40
    int countIdx = enterPacket.get_index();                 // -0x64
    enterPacket.put_byte(liveCount);

    char hackFlag = 0;                                      // -0x3a
    char hostVill = user->getCurCharacVill();               // -0x39
    int hostArea = user->get_area(false);                   // -0x38

    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        char vill = m_member[i].m_pUser->getCurCharacVill();   // -0x1d
        int area = m_member[i].m_pUser->get_area(false);       // -0x1c
        // [G7-sub] P0 槽律：ORIG 0859d8b2..0859d8e8 把 `(hostVill!=vill)||(hostArea!=area)`
        // 的结果只留在 %al（`movzbl -0x39; cmp -0x1d; jne →mov $1/jmp; ...; test %al,%al;
        // je`），**无 bool mismatch 具名槽**；我方原具名落 -0x21 并多出
        // `mov %al,-0x21; cmpb $0x0,-0x21`（2 条 + 4B 帧）⇒ 内联进 if 条件（短路序不变）。
        if (((hostVill != vill) || (hostArea != area)) && !is_quick_party() &&
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                    ->m_villageMonsterId == 0)                  // ORIG this+0xc4c
        {
            LogManager::logFormat(
                1, "party.cpp", __PRETTY_FUNCTION__, 0x84c,
                "[HACK_VILL_AREA] m_id : %s, Vill(%d,%d), Area(%d,%d)",
                NumberToString(m_member[i].m_pUser->get_acc_id(), 0),
                (int)hostVill, (int)vill, hostArea, area);
            hackFlag = 1;
            break;
        }
        m_battleData.m_arr54[i] = 1;      // +0x380
        m_battleData.m_arr58[i] = 1;  // +0x384
        m_battleData.m_arr5c[i] = 0;      // +0x388
        m_battleData.m_arr60[i] = 0;      // +0x38c
        m_member[i].m_pUser->make_basic_info((char*)&basicPacket, 1);
        G_GameWorld()->goto_dungeon(m_member[i].m_pUser);
        if (m_member[i].m_pUser->get_state() == 5)
        {
            enterPacket.put_short(
                m_member[i].m_pUser->get_unique_id() & 0xffff);
            enterPacket.put_byte(1);
            ++liveCount;
        }
        int tradeIdx = m_member[i].m_pUser->GetTradeSpace();   // -0x48
        if (tradeIdx != -1)
        {
            CTradeSpace* ts = G_CGameManager()->GetTradeSpace(tradeIdx);
            if (ts != 0 && ts->check_trade_possibility() != 1)
            {
                ts->cancel_trade();
                G_CGameManager()->PutTradeSpace(ts);
            }
        }
        m_member[i].m_pUser->getCurCharacQuestW()->m_authenData.reset();
    }

    if (hackFlag != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = get_user(i);
            if (u != 0)
            {
                LogManager::logFormat(
                    1, "party.cpp", __PRETTY_FUNCTION__, 0x88a,
                    "[HACK_VILL_AREA] m_id : %s, Vill(%d), Area(%d)",
                    NumberToString(u->get_acc_id(), 0), (int)hostVill,
                    hostArea);
                u->DisConnSig((DISCONN_SIG)0x18, true, 0);
            }
        }
        return;
    }

    enterPacket.put_byte(countIdx, liveCount);
    enterPacket.finalize(true);
    if (!IsExistInvisible())
    {
        if (getManager()->getMoveSpace() != 1)
            G_GameWorld()->send_all(enterPacket);
        else
            send_to_party(enterPacket);
    }
    else
    {
        send_to_party(enterPacket);
    }

    basicPacket.finalize(true);
    send_to_party(basicPacket);

    set_host();
    send_host_info();

    m_field1864 = 0;
    m_hellWorldMap = 0;
    int worldIdx = G_GameWorld()->GetWorldMapIndex(user);   // -0x34
    if (worldIdx > 0)
        m_hellWorldMap =
            G_CDataManager()->m_worldMapList.find_world_map(worldIdx);

    basicPacket.clear();
    basicPacket.put_header(0, 0x1b);

    // P183：ORIG 各使用点重取址（无引用缓存槽）⇒ 宏直用。
#define bd (m_battleData)
    if (get_member_count() == 1)
    {
        bd.SetHellPartyValueTotal(
            static_cast<stHellParty_t*>(
                G_CDataManager()->get_hellparty_script_values())
                ->m_baseValue);   // ORIG game_start.md 0859dd76 `mov 0x8(%eax)`
                                  // = stHellParty_t::m_baseValue
    }

    // ORIG this+0xd0c = CBattle_Field+0x1e8 = m_hellPartyRatio0（int，2026-09-03
    // 头类型修正；见 reset_hell_party_value/send_hellParty_value 报告）。
    int hellDiv = reinterpret_cast<CBattle_Field*>(&m_battleField)->m_hellPartyRatio0;
    int hellVal = (int)(bd.m_hellPartyValueTotal * 100.0f / (float)hellDiv);
    basicPacket.put_int(hellVal);

    std::vector<int> hellQuests;                            // -0x70
    bool hellEnable =
        (m_hellWorldMap != 0 && m_hellWorldMap->IsInHellDungeon() &&
         reinterpret_cast<CBattle_Field*>(&m_battleField)
             ->IsEnableHellDungeon());
    if (hellEnable)
    {
        CheckClearQuestHellParty(hellQuests);
        size_t qn = hellQuests.size();                      // -0x10
        if ((int)qn < 1)
        {
            basicPacket.put_byte(0);
            m_field1864 = 1;
        }
        else
        {
            basicPacket.put_byte((int)qn);
            // ORIG 0x0859de..：迭代器遍历（p167 MISS begin/end/de/pp/ne；我方旧码
            // 下标形产 EXTRA operator[]）。
            for (std::vector<int>::iterator it = hellQuests.begin();
                 it != hellQuests.end(); ++it)
                basicPacket.put_short(*it);
        }
    }
    else
    {
        basicPacket.put_byte(0);
    }

    if (!make_blood_dungeon_info(user, basicPacket))
        make_blood_dungeon_info_blank(basicPacket);

    bool deathTowerArea = false;                            // -0x29
    worldIdx = G_GameWorld()->GetWorldMapIndex(user);       // -0x28
    if (worldIdx > 0)
    {
        CWorldMap* wm2 =
            G_CDataManager()->m_worldMapList.find_world_map(worldIdx);  // -0xc
        (void)wm2;
        if (m_hellWorldMap != 0 && m_hellWorldMap->hasDeathTower())
            deathTowerArea = true;
    }

    if (isDungeonDifficultyInfromNotice(deathTowerArea))
    {
        basicPacket.put_byte(1);
        m_manager->setMemberDungeonFlag(true);
    }
    else
    {
        basicPacket.put_byte(0);
    }
    if (deathTowerArea)
        reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->setWorldMapTowerArea(deathTowerArea);

    basicPacket.finalize(true);
    send_to_party(basicPacket);

    m_recvResultFlag = 0;                                   // +0x118
    // ORIG 0x0859e05x 经 CParty::GetPartyTelePort()（p167 MISS，旧码直取 &m_pad1ad0）。
    reinterpret_cast<CPartyTelePort*>(GetPartyTelePort())->reset_teleport_data();
}