bool DB_SavePvp::SavePvp(SIG_SAVE_PVP* sig)
{
    // 声明序 s→r→db 与 ORIG 栈槽对应（-0x24/-0x1d/-0x1c）：s 为参数副本
    // （ORIG 先存 sig 到 -0x24 再 GetDBHandle）；characNo 直读参数 sig
    // （ORIG 该字段重读 0xc(%ebp)），其余字段经视图强转重读 s。
    SIG_SAVE_PVP* s = sig;
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate pvp_result set win=%d,lose=%d,pvp_point=%d,pvp_grade=%d,avg_kill_count=%d,"
        "avg_buf_count=%d,avg_debuf_count=%d,avg_heal_count=%d,avg_counter_count=%d,"
        "avg_back_atk_count=%d,avg_union_hit_count=%d,avg_overkill_count=%d,avg_combo_count=%d,"
        "avg_aerial_count=%d,avg_attacked_count=%d,avg_deal_damage=%d,avg_technic=%d,"
        "avg_style=%d,avg_hit_penalty=%d,pvp_count=%d,win_point=%d,last_play_time=from_unixtime(%d),"
        " play_count=%u,play_time=%u where charac_no=%u",
        ((DB_SavePvpResultView*)s)->m_win,
        ((DB_SavePvpResultView*)s)->m_lose,
        ((DB_SavePvpResultView*)s)->m_pvpPoint,
        ((DB_SavePvpResultView*)s)->m_pvpGrade,
        ((DB_SavePvpResultView*)s)->m_avg[0],
        ((DB_SavePvpResultView*)s)->m_avg[1],
        ((DB_SavePvpResultView*)s)->m_avg[2],
        ((DB_SavePvpResultView*)s)->m_avg[3],
        ((DB_SavePvpResultView*)s)->m_avg[4],
        ((DB_SavePvpResultView*)s)->m_avg[5],
        ((DB_SavePvpResultView*)s)->m_avg[6],
        ((DB_SavePvpResultView*)s)->m_avg[7],
        ((DB_SavePvpResultView*)s)->m_avg[8],
        ((DB_SavePvpResultView*)s)->m_avg[9],
        ((DB_SavePvpResultView*)s)->m_avg[10],
        ((DB_SavePvpResultView*)s)->m_avg[11],
        ((DB_SavePvpResultView*)s)->m_avg[12],
        ((DB_SavePvpResultView*)s)->m_avg[13],
        ((DB_SavePvpResultView*)s)->m_avg[14],
        ((DB_SavePvpResultView*)s)->m_pvpCount,
        ((DB_SavePvpResultView*)s)->m_winPoint,
        GlobalData::s_systemTime_.getCurSec(),
        ((DB_SavePvpResultView*)s)->m_playCount,
        ((DB_SavePvpResultView*)s)->m_playTime,
        sig->m_characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
