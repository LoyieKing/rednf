// ORIG 0x0844c4e2：GetDBHandle(2,0)==0 返 0；GetOutBuffer 失败返 0；set_query
// cube_premium；exec 失败返 0；Acquire(0xbf71) → <<userIdx → <<slot →
// GetInBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES> → +0=characNo、+4=0、
// +5=0 → fetch 有行时 get_ubyte(0,+4)/get_ubyte(1,+5)（失败返 0）→ put 返 1。
bool DB_GetPremiumSelectedCubeInfo::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    SIG_GET_PREMIUM_SELECTED_CUBE_INFO* sig =
        stream->GetOutBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO>();
    if (sig == 0)
        return 0;
    db->set_query("seLect selected, cube_type from cube_premium where charac_no=%u",
                  sig->m_characNo);
    // ORIG 0x0844c4e2：`mov %al,-0xd; movzbl -0xd,%eax; xor$1;test` ——
    // exec 结果经 bool 槽 -0xd spill+reload（batch-8 InformNotice 技巧，
    // 该 byte 槽同时把 db/sig/guard 整体外推 4B：ORIG db=-0x18、
    // sig=-0x14、guard=-0x20）。
    bool r;
    r = db->exec(true);
    if (!r)
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbf71), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES* out =
        guard->GetInBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES>();
    out->m_characNo = sig->m_characNo;
    out->m_selected = 0;
    out->m_cubeType = 0;
    // ORIG 0x0844c…：三处 bool 槽 -0xd spill——fetch 正向 `cmpb $0;je`、
    // get_ubyte×2 `movzbl;xor$1;test;je`（技巧1/8 同槽复用）。
    r = db->fetch();
    if (r)
    {
        r = db->get_ubyte(0, out->m_selected);
        if (!r)
            return 0;
        r = db->get_ubyte(1, out->m_cubeType);
        if (!r)
            return 0;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}