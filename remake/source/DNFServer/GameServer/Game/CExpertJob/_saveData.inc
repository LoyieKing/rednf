// _saveData 0x0832b0ac：StreamGuard 写 0x181 头 + UID + SIG_EXPERT_JOB 载荷
// （DB wire 格式，结构定义见 SigTypes.h），MsgQueueMgr::put(DB_Q)。
// [b28d-solo-CExpertJob] AE（92=92，DIFF→AE）：P211 档③/P235 匿名 union 双视角
//   （SigTypes_B.h SIG_EXPERT_JOB::m_characNoU，与 getCurCharacNo() 同型）复现
//   ORIG 0x0832b163 `mov -0xc(%ebp),%edx; mov %eax,(%edx)` LHS-first 直存；
//   消费点 W8LT_DBTask.cpp 读 int 视角零回归（属主 S340 fresh 复验确认）。ORIG 0x0832b0ac
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
}