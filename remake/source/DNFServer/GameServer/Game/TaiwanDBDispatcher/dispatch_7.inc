// ORIG 0x0815d024：GetOutBuffer<MSG> null → throw "context null"；GetDBHandle
// (9,0) null → throw "handle null"；set_query("seLect m_id from
// pcroom_daily_reward_tw where m_id=%s", NumberToString(msg->m_id))；exec 失败
// throw "select_exec"；get_n_rows()==0 → INSERT(values('%s', now(), %d),
// str, (char)msg->m_remain) exec 失败 throw "insert_exec"；否则 UPDATE
// (remain_reward_count=%d where m_id = %s, str, (char)m_remain) exec 失败
// throw "update_exec"。catch logFormat(0x35d) 返 0。成功返 1。
// [b28d-lane6] AE（已修·真 bug）：UPDATE 语句实参序 (str,remain)→(remain,str) 修正（ORIG 0x0815d0f2 压栈序实证）+ 删多余 set_query 检查 + remain 内联。
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
}
