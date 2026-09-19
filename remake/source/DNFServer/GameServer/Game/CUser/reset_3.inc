void CUser::reset()
{
    ResetCurCharac();
    m_gmBossDifficulty = 0;
    m_gmBossMapIndex = 0;
    m_field8cf00 = 0;
    m_field8ead8 = 0;
    m_premium.InitPremium();
    m_characList.clear();
    m_characList.reserve(8);
    m_guildAgitDB.m_pad[2] = 0;
    m_characterView.reset();
    m_hackAnalyzer.resetHackInfo();
    for (int i = 0x65; i < 0xfa1; ++i)
        getHackAnalyzer()->resetServerHackAccumulatedCnt((WongWork::ENUM_HACKTYPE)i);
    m_accHacking.resetInfo();
    if (m_premium.m_mailBox != 0)
    {
        WongWork::CMailBoxHelper::FreeMailBox((WongWork::CMailBox*)m_premium.m_mailBox);
        m_premium.m_mailBox = 0;
    }
    m_list711ec.clear();
    InitFinishPoint();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    m_guildAgitDB.m_pad[1] = 0;
    m_codeHackCheck.reset();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
    setUserEventCharacterFlag(0);
    m_vec8cef4.clear();
    m_field8d0f0 = 0;
    m_field8d0f1 = 0;
    m_field8d0f2 = 0;
    m_field8e074 = 0;
        m_character.PlayInfo::Reset();
    m_field8d0f8 = 0;
    m_field8d0f4 = 0;
    m_379.reset();
    setAntibotKey(-1);
    setClientVersion(0);
    m_dungeonGainedGold.reset();
    m_postChecksumData.m_field44 = 0;
    m_rating = 0;
    m_field8cfc0 = 0xffffffff;
    m_postChecksumData.m_totalFatigueUsed = 0;
    m_levelupEvents.clear();
    m_characExpand.reset();
    m_premium.m_field98c = 0;
    m_limitItemUsage.clear();
    m_quest.m_field761c = 0;
    m_accId = 0;
    // ORIG reset 0x864a5c0+164..195 语句序：59 → accountCargo.Clear → 5c(0x7965c)
    // → 5e → 60 → dup(0x7966c) → str0 → str1 → 58(0x7965c int) → 56。
    m_postChecksumData.m_field59 = 0;
    m_accountCargo.Clear();
    m_postChecksumData.m_field60 = 0;
    m_postChecksumData.m_field62 = 0;
    m_postChecksumData.m_field64 = 0;
    m_dupMsgSlotIdx = 0;
    m_str79664[0].clear();
    m_str79664[1].clear();
    m_postChecksumData.m_field5c = 0;
    m_postChecksumData.m_field5a = 0;
    ResetUsedCoinInDungeon();
    DBUpdateDBLogItem();
    ResetDBLogItem();
    m_actionPoint.Reset();
    CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
    reset_server_fatigue_data();
    reset_ingame_event_history();
    reset_ingame_event_history_update();
    m_mercenaryMgr.m_characs.clear();
#if !remake_verify
    // [DNF-FIX-GAMEOPTION] CGameOption 构造只初始化 2 个标志位，其余字段是未初始化内存；
    // 而 GAME_OPTION(notify 173) 会把整个结构体序列化给客户端 ⇒ 客户端解析到垃圾后
    // 内部出错（右上角等级显示 16/211 之类的乱值、角色界面按钮全部失效）。
    // 登录路径必经 reset()，此处整块清零，后续各字段再按原逻辑单独赋值。
    std::memset(&m_gameOption, 0, sizeof(m_gameOption));
#endif
    m_field6ef68 = 0;
    m_field6ef69 = 0xff;
    resetMoneyLog();
    m_field4 = 0;
    m_field5 = 0;
    m_receivedRequestType = 0xff;
    m_field796d8 = 0;
    m_quest.reset();
    m_field8ebd8 = 0;
    m_field8ebd4 = m_field8ebd8;
    m_field8ebd0 = m_field8ebd4;
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_bingo.clear();
    m_levelUpGift.m_map.clear();
    m_growthCreature.m_vec.clear();
    GetGameOptionRef()->reset_load_hotkey_flag();
    m_rentalFlag3 = 0;
    m_field8ec31 = 0;
    m_field8ec32 = 0;
}