bool DB_AuctionSubMoney::dispatch(int userIdx, int slot, Stream* stream)
{
    // ORIG 0x08448530 语句序（P165 物理展开）：GetOutBuffer → mId=0 →
    // GetDBHandle → buf[0x3b] 零初始化（GCC 对 0x3b 错位缓冲按 &1/&2/rep 分块）。
    SIG_AUCTION_MONEY_SUB* sig =
        stream->GetOutBuffer<SIG_AUCTION_MONEY_SUB>();
    // P248 档③：m_count 为 int ⇒ 局部同型化（unsigned→int）以消 4.4 的
    // `mov %eax,%edx` 符号转换中转（ORIG 0x08448530 该 store 为单段 RHS-first）。
    // [b28d-solo] P167：ORIG 调 `MySQL::get_int(int, unsigned int&)`
    // （差集 -1 get_int(Rj)/+1 get_int(Ri)）；但 unsigned 局部与 m_count(int)
    // 异型会多 `mov %eax,%edx` 中转（diff 1→6，AE 回归）⇒ 维持 int 同型，
    // 重载差（语义等价，位模式相同）记 N。
    int mId = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    char buf[0x3b] = { '\0' };
    db->escape_string(buf, sig->m_name);
    db->set_query("seLect m_id from charac_info where charac_name='%s' limit 1",
                  buf);
    if (!db->exec(true))
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_int(0, (unsigned int&)mId))
        return 0;
    // ORIG 0x0844866c(+0x13c)：CStreamGuard 在 get_int 之后才构造（前移会改
    // 帧布局与语句序，P165 物理展开序），非函数顶部。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb551), true);
    guard.operator*()->operator<<(0x309);
    guard.operator*()->operator<<(slot);
    // [b22] GetInBuffer 走 operator->（ORIG ptEv + GetInBuffer），非 operator*
    // （P132：operator-> 链保留，P167 显示 pt-1/de+1 实缺）。
    SIG_AUCTION_MONEY_SUB* out =
        guard.operator->()->GetInBuffer<SIG_AUCTION_MONEY_SUB>();
    out->m_count = mId;
    out->m_arg04 = sig->m_arg04;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
