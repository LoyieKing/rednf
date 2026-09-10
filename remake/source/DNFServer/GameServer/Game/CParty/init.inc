void CParty::init()
{
    // b28d-solo-CParty2：pad 区已字段化（m_traceMobDieHack），余差 N82 帧/槽。@0x859a974
    // [G7-sub 复核] 全函数仅 1 处实质差：ORIG 0859aa5f `lea 0x1(%eax),%edx` vs
    // 我方 `add $0x1,%eax; mov %eax,%edx`（m_titleIndex = get_rand_int(0xff)+1）——
    // 4.1 的 lea-fold 对 4.4 不成立；前轮已试 (unsigned char) 窄化/(char) 截断/直赋
    // 三形（均产 add+mov，203→203 不降）⇒ 判 N1 封顶，本轮不重复试形。
    m_traceMobDieHack.reportHackInfo();
    m_traceMobDieHack.reset();
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        m_member[i].m_pUser->getHackAnalyzer()->reportHackInfo();
        m_member[i].m_pUser->getHackAnalyzer()->resetHackInfo();
    }
    m_memberLevelGap = -1;
    m_recvResultFlag = 0;
    memset(m_title, 0, 0x20);
    // b28d-solo-CParty：ORIG 0x859ab7a `lea 0x1(%eax),%edx`（P30/N1：4.4 对
    // int+1 恒折 add+mov）。试形记录（已回滚）：(unsigned char) 窄化 / (char)
    // 截断 / 目标 char 直赋 三形均产 add+mov 且总数 201→203 反劣 ⇒ 判 N1。
    m_titleIndex = get_rand_int(0xff) + 1;
    m_userMax = 4;
    m_dungIndex = (unsigned short)0xffff;
    m_dungDiffi = 0xff;
    m_field68 = 0;
    m_state = 0;
    m_singlePlay = false;
    m_autoCreated = 0;
    SetEPLPState(0);
    m_selectedEPLPCmd = (char)0xff;
    SetAssaultState(0);
    for (int i = 0; i <= 3; ++i)
    {
        m_member[i].Init();
        for (int j = 0; j <= 1; ++j)
            m_field204[j][i] = (char)0xff;
        m_dungeonClearState = 0;
    }
    for (int i = 0; i <= 1; ++i)
        for (int j = 0; j <= 3; ++j)
            m_field1e4[i][j] = 0;
    m_battleData.SetHellPartyValueTotal(
        static_cast<stHellParty_t*>(
            G_CDataManager()->get_hellparty_script_values())
            ->m_baseValue);   // ORIG init.md 0859ab9f `mov 0x8(%eax)` =
                              // stHellParty_t::m_baseValue（DataManagerInitDepends.h
                              // importHellPartyScript 0x8909857 [base value] 实证）
    reinterpret_cast<CBattle_Field*>(&m_battleField)->reset_hell_party_value();
    m_battleData.Reset();
    reinterpret_cast<CBattle_Field*>(&m_battleField)->reset_field();
    m_recvFlag.Clear();
    m_usedCoinCount = 0;
    m_hellWorldMap = 0;
    m_straightVictories = 0;
    m_secretShop.clear();
    ResetPremiumGoldCardParty();
    ResetPremiumGoldCardDefaultItem();
    m_weekendEvent = 0;
    m_randomBuffType = (QuickParty::RandomBuffType)0xb;
    init_quick_party_data();
    m_tournamentDungeonClearState = 0;
    m_dungeonMapSaving = 0;
