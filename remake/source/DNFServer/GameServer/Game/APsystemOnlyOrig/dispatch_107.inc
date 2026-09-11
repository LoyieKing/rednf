// ============================================================================
// DB_AuctionSubMoney —— ORIG dispatch 0x08448530。
// 报告：docs/class_func_reports/DB_AuctionSubMoney/dispatch.md。
// dispatch：DB_HANDLE_2；escape_string(m_name) 后按角色名查 m_id（exec/fetch/get_int
// 任一失败返 0）→ 组包（0x309/slot，写 m_count=m_id、m_arg04=sig+4）→
// put(DISPATCH_Q) 返 1。
// b28c-28 判词（N18a）：fresh 唯一残差 = `out->m_count = mId` 处 ORIG
// 0x08448704 `mov -0x28(%ebp),%edx`（一步直落目标寄存器）vs 4.4 恒
// `mov -0x28,%eax; mov %eax,%edx`（两段，+1 insn）。探针 /tmp/l28_probe6.cpp
// h1/h2/h3（unsigned 槽→int 字段 / 形参直传 / 双字段）实测：仅「值未经 call
// 边界」时可一步，call 后装载恒两段 ⇒ N18a 封顶，余 147/147 全等。
// ============================================================================
// [b28d-4] AE：P248 档③ 局部 mId unsigned→int 同型化，消 `mov %eax,%edx`
// 符号转换中转，复现 ORIG 0x08448530 单段直落 store（156=156 全等）。
// [b28d-solo-3] 复核：int 同型化后 p167 显示 get_int 重载差（MISSING Rj /
// EXTRA Ri）；改调用点 `(unsigned int&)mId` 直传 ⇒ p167 CLEAR 且形态保持
// 156=156 AE（int 槽与 int 字段同型，仅取址类型切换，位模式不变）。
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
}