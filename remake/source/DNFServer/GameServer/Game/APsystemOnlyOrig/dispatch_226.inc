// ============================================================================
// DB_AutoPunishedSecondUserData::dispatch —— ORIG 0x0843a462
// 查询 auto_punish_second_user：无数据发 0x88f1 失败包返 0；有数据发 0x8901
// 包（trade_cnt+1、trade_gold 直传）返 1。出站头 0x1ca 前导 m_id。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843a462
bool DB_AutoPunishedSecondUserData::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    // [H4 2026-09-11] P171 死存储照抄：ORIG 0x0843a46a `movb $0x0,-0x19(%ebp)`
    // 位于函数首条（该槽全函数只写不读）⇒ 原码函数头遗留的 bool 声明预清。
    bool deadFlag = false;
    (void)deadFlag;
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
        // [K5W8LTDBTask W8DiagD 负结论] u64 别名拷贝旋钮（P246/P92）实测无变化
        // （our 215 未动、四数 141/1750/12/57 逐值相同）：4.4 对
        // `*(unsigned long long*)&a = *(unsigned long long*)&b` 与逐字段赋值同码。
        // ORIG 0x0843a5b3-0x0843a5b6 的 6 条 8 字节拷贝属 4.1 后端，不可迁。回滚。
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
    // P171/P328⑤：ORIG 此处（Acquire 之后）无 `movl $0x0,-0x1c/-0x24/-0x28`
    // 预清（裸声明只占槽、不发指令；两值随后由 get_uint/get_ulonglong 写入）。
    unsigned int tradeCnt;
    unsigned long long tradeGold;
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
    // [J4S3-15 P246] ORIG 0x0843a715 一次 8 字节拷贝（`mov 0x8(%eax),%edx;
    // mov 0x4(%eax),%eax; mov -0x10(%ebp),%ecx; mov %eax,(%ecx); mov %edx,0x4(%ecx)`）、
    // 0x0843a723 一次 8 字节写 ⇒ 视图整体拷贝（out+0/+4 与 req goldLo/goldHi 同布局）。
    *(unsigned long long*)&out->m_goldLo = *(unsigned long long*)&req->m_goldLo;
    *(unsigned long long*)&out->m_tradeCnt = tradeGold;
    out->m_punished = tradeCnt + 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}