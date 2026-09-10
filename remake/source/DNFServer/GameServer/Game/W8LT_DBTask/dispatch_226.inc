bool DB_AutoPunishedSecondUserData::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_AutoPunishedSecondUserReqView* req =
        reinterpret_cast<DB_AutoPunishedSecondUserReqView*>(
            stream->GetOutBuffer<SIG_SECOND_USER_DATA>());
    db->set_query(
        "seLect trade_cnt,trade_gold from auto_punish_second_user where m_id=%s",
        NumberToString(req->m_mId, 0));
    // ORIG：exec 失败或 rows==0 → 失败路径（0x88f1 包，return 0）。
    if (!(db->exec(true) && db->get_n_rows() != 0))
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x88f1), true);
        guard.operator*()->operator<<(0x1ca);
        guard.operator*()->operator<<(req->m_tradeCnt);
        DB_AutoPunishedSecondUserSetView* out =
            reinterpret_cast<DB_AutoPunishedSecondUserSetView*>(
                guard->GetInBuffer<SIG_SECOND_USER_DATA_SET>());
        memset(out, 0, sizeof(*out));
        // ORIG 0x0843a4fd 失败分支逐字段（report dispatch.md）：
        // out+0x14=req+0x10、out+0=req goldLo、out+0x4=req goldHi、
        // out+0x8=0、out+0xc=0、out+0x10=1（punished 硬编码 1，非 req）。
        out->m_mId = req->m_punished;
        out->m_goldLo = req->m_goldLo;
        out->m_goldHi = req->m_goldHi;
        out->m_tradeCnt = 0;
        out->m_pad0c = 0;
        out->m_punished = 1;
        // ORIG 0x0843a5e8(+0x186)：put 队列 = DISPATCH_Q(1)，非 DB_Q(2)
        // （两分支同；见 report dispatch.md 反汇编 movl $0x1,0x4(%esp)）。
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 0;
    }
    db->fetch();
    unsigned int tradeCnt = 0;
    unsigned long long tradeGold = 0;
    db->get_uint(0, tradeCnt);
    db->get_ulonglong(1, tradeGold);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8901), true);
    guard.operator*()->operator<<(0x1ca);
    guard.operator*()->operator<<(req->m_tradeCnt);
    DB_AutoPunishedSecondUserSetView* out =
        reinterpret_cast<DB_AutoPunishedSecondUserSetView*>(
            guard->GetInBuffer<SIG_SECOND_USER_DATA_SET>());
    memset(out, 0, sizeof(*out));
    // ORIG 0x0843a4fd 成功分支逐字段（report dispatch.md）：
    // out+0x14=req+0x10、out+0=req goldLo、out+0x4=req goldHi、
    // out+0x8=tradeGold 低 32（DB col1）、out+0xc=tradeGold 高 32、
    // out+0x10=tradeCnt+1。
    out->m_mId = req->m_punished;
    out->m_goldLo = req->m_goldLo;
    out->m_goldHi = req->m_goldHi;
    out->m_tradeCnt = (unsigned int)(tradeGold & 0xffffffffULL);
    out->m_pad0c = (unsigned int)(tradeGold >> 32);
    out->m_punished = tradeCnt + 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
