// ORIG 0x84172c8：读 m_result 前 0x68 的 22 个 int + CSystemTime::getCurSec()
// + charac_no，拼 25 占位 SQL（win..win_point/from_unixtime/play_count/play_time/
// charac_no）；exec 失败返 0。
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
        s->m_win,
        s->m_lose,
        s->m_pvpPoint,
        s->m_pvpGrade,
        s->m_avg[0],
        s->m_avg[1],
        s->m_avg[2],
        s->m_avg[3],
        s->m_avg[4],
        s->m_avg[5],
        s->m_avg[6],
        s->m_avg[7],
        s->m_avg[8],
        s->m_avg[9],
        s->m_avg[10],
        s->m_avg[11],
        s->m_avg[12],
        s->m_avg[13],
        s->m_avg[14],
        s->m_pvpCount,
        s->m_winPoint,
        GlobalData::s_systemTime_.getCurSec(),
        s->m_playCount,
        s->m_playTime,
        sig->m_characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}