// ORIG 0x0844d17c：*stream>>mId(uint)；GetDBHandle(6,0)；seLect m_id from
// allow_proxy_user where m_id=%s；exec 成功且 get_n_rows()==0 →
// Acquire(0xc2b7) → <<0x352 → <<slot → put(DISPATCH_Q)；返 1。
bool DB_SelectAllowProxyUser::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int mId = 0;
    *stream >> mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect m_id from allow_proxy_user where m_id=%s",
                  NumberToString(mId, 0));
    // ORIG 0x844d17c：exec 短路 `test;je` 后，第二条件值物化
    // `test %eax,%eax; sete %al; test %al,%al; je`。4.4 在 && 中把物化消解成
    // jne+mov$1/mov$0 岛；拆嵌套 if 保 (bool) 物化形（技巧 9）。
    if (db->exec(true))
        if ((bool)(db->get_n_rows() == 0))
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc2b7),
                true);
            guard.operator*()->operator<<((int)0x352);
            guard.operator*()->operator<<((int)slot);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
    return 1;
}