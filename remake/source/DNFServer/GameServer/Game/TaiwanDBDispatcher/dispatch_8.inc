// ORIG 0x0815d3ee：GetOutBuffer<MSG> null → throw "context null"；GetDBHandle
// (9,0) null → throw "handle null"；time+localtime_r 拷贝 tm，tm_hour 覆为
// m_fatigueTime、tm_min/tm_sec 清零，mktime；原始 tm_hour < fatigueTime 时减
// 86400；set_query("seLect m_id from event_1306_account_reward_2nd where m_id=%s
// and occ_date > FROM_UNIXTIME(%d)", str, now) exec 失败 throw "select_exec"；
// get_n_rows()==0 → INSERT(m_id,charac_no,occ_date) exec 失败 throw
// "insert_exec"；Acquire(file, 0x3a5) → CStreamGuard(true) →
// internal_stream(guard, 0x10, uid) → GetInBuffer<MSG> null → throw "stream"；
// 拷贝 m_characNo/m_accId 后 MsgQueueMgr::put(idx 1, guard)。catch
// logFormat(0x3b4) 返 0。成功返 1。
// [b21] 修正三真 bug：①set_query 返回不检查（ORIG 仅 check exec，删 if(!..)throw 两支）；
//   ②MsgQueueMgr::put 旧用 DB_Q(=2)，ORIG 传 $0x1（同族 Taiwan dispatch 均 idx 1，改 DISPATCH_Q）；
//   ③m_accId/m_characNo 拷贝序反（ORIG 先 0x4 后 0x0）。疲劳比较改读 tm2.tm_hour 内存对
//   齐 `cmp/jge`；Acquire 内联进 CStreamGuard ctor（P134）消中间槽；nRows 具名局部。
// [b28c-L20] AE — 消上列两处残余（原「封 N82」判词作废）：
//   ①m_characNo 装载：写 (unsigned int)sig->m_characNo 的异型化实参使 4.4 多
//   `mov %eax,%ebx` 中转；保持字段 int 同型直传即复现 ORIG `mov 0x4(%eax),%ebx`（P211 反向）；
//   ②返回路径：ORIG 成功值 `mov $1,%ebx` 落在函数尾共享块（0x0815d797，guard 析构之后、
//   与 catch 的 mov $0,%ebx 汇合）⇒ return 1 必须写在 try 之外。
//   0x0815d3ee orig=231 our=231 gap=0 ndiff=0。
bool DBInsertGiveBoxEventReward::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBInsertGiveBoxEventReward::MSG* sig = stream->GetOutBuffer<DBInsertGiveBoxEventReward::MSG>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        time_t t = time(0);
        struct tm tm;
        localtime_r(&t, &tm);
        struct tm tm2 = tm;
        tm2.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        tm2.tm_min = 0;
        tm2.tm_sec = 0;
        time_t now = mktime(&tm2);
        if (tm.tm_hour < tm2.tm_hour)
            now -= 0x15180;
        db->set_query(
                "seLect m_id from event_1306_account_reward_2nd where m_id=%s and occ_date > FROM_UNIXTIME(%d)",
                NumberToString(sig->m_accId, 0), (int)now);
        if (!db->exec(1))
            throw "select_exec";
        int nRows = db->get_n_rows();
        if (nRows == 0)
        {
            // b28c-L20: ORIG 0x0815d5c6 对 m_characNo(int) 直发
            // `mov 0x4(%eax),%ebx`（跨 NumberToString 调用保活）；写
            // `(unsigned int)sig->m_characNo` 的异型化实参会多一条
            // `mov %eax,%ebx` 中转 ⇒ 保持字段同型（int）直传（P211 反向）。
            db->set_query(
                    "inSert into event_1306_account_reward_2nd (m_id , charac_no , occ_date ) values (%s , %u , now())",
                    NumberToString(sig->m_accId, 0), sig->m_characNo);
            if (!db->exec(1))
                throw "insert_exec";
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire(
                    "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x3a5),
                true);
            Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x10, uid);
            DBInsertGiveBoxEventReward::MSG* outSig =
                guard.operator->()->GetInBuffer<DBInsertGiveBoxEventReward::MSG>();
            if (outSig == 0)
                throw "stream";
            outSig->m_characNo = sig->m_characNo;
            outSig->m_accId = sig->m_accId;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x3b4,
            "[Taiwan, GiveMeBox] GIVE_ME_BOX_EVENT. (error:%s)", e);
        return 0;
    }
    // b28c-L20: ORIG 0x0815d3ee 成功返回值在函数尾共享块物化（mov $1,%ebx 位于
    // CStreamGuard 析构之后、与 catch 的 mov $0,%ebx 汇合）⇒ return 1 落在
    // try 之外，不能写在 try 块内（4.4 会在 put 后立刻物化，ndiff 5）。
    return 1;
}
