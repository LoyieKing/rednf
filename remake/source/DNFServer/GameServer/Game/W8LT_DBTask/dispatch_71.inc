// ============================================================================
// DB_LoadExpertJob::dispatch —— ORIG 0x08436464（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。*stream >> characNo(uint) → seLect expert_job_giveup_cnt/
// expert_job_info/expert_job_info_ex/recipe from charac_expert_job where
// charac_no=%d；exec 失败返 0。
//   get_n_rows()==0 → 组包（Acquire("DBThread.cpp",0x81b2)）<<userIdx<<slot →
//   GetInBuffer<SIG_EXPERT_JOB> → memset 0x30 → put(DISPATCH_Q) 返 1；
//   否则 fetch 失败返 0；成功再组包（0x81c2）→ memset 0x30 →
//   get_ushort(0,+4)/get_int(1,+8)/get_int(2,+0xc)/get_binary(3,+0x10,0x1e)，
//   任一失败 StreamPool::Free(guard) 返 0；全成 put(DISPATCH_Q) 返 1。
// （N 封顶：ndiff 176 残差为 CStreamGuard try/catch EH 区域 + guard 帧布局
// （-0x20 vs -0x28）+ 嵌套失败各 Free 的汇聚差异，见报告 dispatch.md）
// [b28d-4] NEAR：NEAR 封顶：助记符序列全同、仅栈槽地址/寄存器文本差（N82 槽号错位 / N7 寄存器轮换）；ORIG 08436464
bool DB_LoadExpertJob::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    *stream >> characNo;
    db->set_query(
        "seLect expert_job_giveup_cnt, expert_job_info, expert_job_info_ex, recipe  from charac_expert_job where charac_no=%d",
        characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x81b2), true);
        guard.operator*()->operator<<(userIdx);
        guard.operator*()->operator<<(slot);
        SIG_EXPERT_JOB* sig =
            guard.operator->()->GetInBuffer<SIG_EXPERT_JOB>();
        memset(sig, 0, 0x30);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    if (db->fetch() != 1)
        return 0;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x81c2), true);
        guard.operator*()->operator<<(userIdx);
        guard.operator*()->operator<<(slot);
        SIG_EXPERT_JOB* sig =
            guard.operator->()->GetInBuffer<SIG_EXPERT_JOB>();
        memset(sig, 0, 0x30);
        if (!db->get_ushort(0, sig->m_giveupCount))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_int(1, sig->m_endurance))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_int(2, sig->m_enchantCount))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_binary(3, sig->m_recipeFlag, 0x1e))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}