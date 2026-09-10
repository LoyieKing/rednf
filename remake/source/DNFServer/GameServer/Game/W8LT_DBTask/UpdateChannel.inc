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
        ((DB_UpdateChannelView*)sig)->m_gcNow,
        ((DB_UpdateChannelView*)sig)->m_swordmanCnt,
        ((DB_UpdateChannelView*)sig)->m_fighterCnt,
        ((DB_UpdateChannelView*)sig)->m_gunnerCnt,
        ((DB_UpdateChannelView*)sig)->m_mageCnt,
        ((DB_UpdateChannelView*)sig)->m_priestCnt,
        ((DB_UpdateChannelView*)sig)->m_atGunnerCnt,
        ((DB_UpdateChannelView*)sig)->m_thiefCnt,
        ((DB_UpdateChannelView*)sig)->m_hangame,
        ((DB_UpdateChannelView*)sig)->m_nexon,
        ((DB_UpdateChannelView*)sig)->m_gcNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
