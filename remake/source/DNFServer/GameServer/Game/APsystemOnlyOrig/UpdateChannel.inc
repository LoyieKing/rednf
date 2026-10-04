// ORIG 0x8418032：gc_now(+0x04 short 符号扩展) + 8 职业计数 + hangame/nexon
// + where gc_no，11 占位 SQL；exec 失败返 0。
bool DB_UpdateChannel::UpdateChannel(SIG_UPDATE_CHANNEL* sig)
{
    // 声明序 r→db 与 ORIG 栈槽对应（-0x1d/-0x1c）；每字段直接经视图强转
    // 访问参数 sig（ORIG 重读 0xc(%ebp)，P48 无 CSE）。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate game_channel set gc_now=%d,gc_up_time=now(),gc_swordman_cnt=%d,gc_fighter_cnt=%d,"
        "gc_gunner_cnt=%d,gc_mage_cnt=%d,gc_priest_cnt=%d,gc_at_gunner_cnt=%d,gc_thief_cnt=%d,"
        "gc_hangame=%d,gc_nexon=%d where gc_no=%d",
        sig->m_gcNow,
        sig->m_swordmanCnt,
        sig->m_fighterCnt,
        sig->m_gunnerCnt,
        sig->m_mageCnt,
        sig->m_priestCnt,
        sig->m_atGunnerCnt,
        sig->m_thiefCnt,
        sig->m_hangame,
        sig->m_nexon,
        sig->m_gcNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}