bool DB_UpdateFairPvPScore::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    static const int handleIdx = 3;
    // 4.4 先声明占更低槽：outStr(-0x28) ok(-0x21) sig(-0x20) db(-0x1c)，
    // 对齐 ORIG 0x843fa9x（ok=exec 结果字节槽，`mov %al,-0x21; movzbl
    // -0x21,%eax; xor $1; test` 物化形，见 0x843fbxx）。
    char* outStr;                    // -0x28（put_compressed_blob_data 出参）
    bool ok;                         // -0x21
    DB_FairPvPScoreView* sig;        // -0x20
    MySQL* db;                       // -0x1c
    sig = (DB_FairPvPScoreView*)stream
              ->GetOutBuffer<SIG_FAIR_PVP_SCORE>();
    if (!sig)
        return 0;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    outStr = 0;
    // ORIG 0x843faax `mov -0x20,%eax; add $0x2c,%eax` —— 4.4 指针加法恒折
    // `lea off(%eax),%edx` 换寄存器；经 (unsigned long) 域算式阻止折叠
    // （探针 /tmp/w8lt_lea3.o::g7 vs ::g8）。
    if (!put_compressed_blob_data(
            db, 0, (char*)((unsigned long)sig + 0x2cUL), 0x650, &outStr))
        return 0;
    db->set_query(
        "upDate fair_pvp_score set private_win=%u,private_lose=%u,private_draw=%u,relay_battle_win=%u,relay_battle_lose=%u,relay_battle_draw=%u,relay_battle_2kill=%u,relay_battle_3kill=%u,successive_win=%u,max_successive_win=%u,job_score='%s',daily_play_count =%u, last_play_time=from_unixtime(%d),give_item=%d where charac_no=%u",
        sig->m_privateWin, sig->m_privateLose, sig->m_privateDraw,
        sig->m_relayBattleWin, sig->m_relayBattleLose, sig->m_relayBattleDraw,
        sig->m_relayBattle2Kill, sig->m_relayBattle3Kill,
        sig->m_successiveWin, sig->m_maxSuccessiveWin, outStr,
        sig->m_dailyPlayCount, sig->m_lastPlayTime,
        (unsigned int)sig->m_giveItem, sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
        return 0;
    return 1;
