// ORIG 0x0843199a：DBDispatcher::dispatch 失败返 0；GetDBHandle(2,0)；
// *stream>>characNo；seLect best_round,best_time,dungeon_index ... 无数据
// trace(0x78e4,0)；有数据逐行 get_int/get_uint → Acquire(0x78f2) → <<0x13f →
// <<slot → SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD(+0 characNo/+4 best_time/
// +8 best_round/+c dungeon_index) → put(DISPATCH_Q)；返 1。
bool DB_LoadBloodBestRecord::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    unsigned int bestTime = 0;
    int bestRound = 0;
    int dungeonIndex = 0;
    *stream >> characNo;
    db->set_query(
        "seLect best_round, best_time, dungeon_index from charac_blood_best_record where charac_no=%u",
        characNo);
    if (!db->exec(true))
        return 0;
    if (!(db->fetch() == 1 && db->get_n_rows() != 0))
    {
        // 技巧3：临时链式（具名块对象把 trace 槽外推至帧底 -0x34，4 ints
        // 反占近槽；ORIG 0x0843199a trace=-0x24 在 db=-0x14 与 ints
        // -0x28..-0x34 之间，先例 DB_LoadPowerWarStatueInfo::dispatch）。
        cMyTrace(__PRETTY_FUNCTION__, 0x78e4, 0)(
            "Not exist data charac_blood_best_record. [charac_no:%d]", characNo);
        return 1;
    }
    for (unsigned int i = 0; i < db->get_n_rows(); ++i)
    {
        if (!db->get_int(0, bestRound))
            return 0;
        if (!db->get_uint(1, bestTime))
            return 0;
        if (!db->get_int(2, dungeonIndex))
            return 0;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x78f2), true);
        guard.operator*()->operator<<((int)0x13f);
        guard.operator*()->operator<<((int)slot);
        SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD* out =
            guard->GetInBuffer<SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD>();
        out->m_characNo = characNo;
        out->m_bestTime = bestTime;
        out->m_bestRound = bestRound;
        out->m_dungeonIndex = dungeonIndex;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 1;
}