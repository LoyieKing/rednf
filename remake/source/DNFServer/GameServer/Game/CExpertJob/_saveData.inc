bool CCharacExpertJob::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CharacExpertJob.cpp", 0x16),
        true);
    **guard << 0x181;
    **guard << (int)pUser->GetUID();
    SIG_EXPERT_JOB* buf = guard->GetInBuffer<SIG_EXPERT_JOB>();
    memset(buf, 0, sizeof(SIG_EXPERT_JOB));   // wire 载荷整体清零（含 pad）
    // [b28d-solo-CExpertJob P211档③] getCurCharacNo() 返回 unsigned int；写 int 字段
    // 异型 ⇒ 4.4 恒 RHS-first+`mov %eax,%edx` 中转（多 1 条）。改走同址 unsigned
    // 视角（SigTypes_B.h union）后与 ORIG 0x0832b163 的 LHS-first 直存逐条全等。
    buf->m_characNoU = pUser->getCurCharacNo();
    buf->m_giveupCount = m_giveupCount;
    buf->m_endurance = m_endurance;
    buf->m_enchantCount = m_enchantCount;
    memcpy(buf->m_recipeFlag, m_recipeFlag, 0x1e);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
