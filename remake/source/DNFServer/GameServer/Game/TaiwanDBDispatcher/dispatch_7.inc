bool DBUpdatePcroomDailyReward::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBUpdatePcroomDailyReward::MSG* sig = stream->GetOutBuffer<DBUpdatePcroomDailyReward::MSG>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // b28d-lane6：不检查 set_query 返回值（ORIG 0x0815d024 仅 check exec）；
        // 不设 remain 具名槽（ORIG 各分支内联重读 `movzbl 0x4(%eax); movsbl`）。
        db->set_query("seLect m_id from pcroom_daily_reward_tw where m_id=%s",
                      NumberToString(sig->m_id, 0));
        if (!db->exec(1))
            throw "select_exec";
        if (db->get_n_rows() == 0)
        {
            db->set_query(
                "inSert into pcroom_daily_reward_tw(m_id, occ_date, remain_reward_count) values('%s', now(), %d)",
                NumberToString(sig->m_id, 0), (int)(char)sig->m_remain);
            if (!db->exec(1))
                throw "insert_exec";
        }
        else
        {
            // b28d-lane6 真 bug 修复：SQL 格式符序为 %d(remain) 后 %s(m_id)，
            // ORIG 0x0815d0f2 压栈序 `mov %edx(remain),0x8(%esp)` +
            // `mov %eax(str),0xc(%esp)` 证实实参序必须是 (remain, str)，
            // 原码 (str, remain) 会把字符串喂给 %d、整数喂给 %s。
            db->set_query(
                "upDate pcroom_daily_reward_tw set occ_date=now(), remain_reward_count=%d where m_id = %s",
                (int)(char)sig->m_remain, NumberToString(sig->m_id, 0));
            if (!db->exec(1))
                throw "update_exec";
        }
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x35d,
            "[Taiwan, Pcroom] pcroom_daily_reward_tw. (error:%s)", e);
        return 0;
    }
