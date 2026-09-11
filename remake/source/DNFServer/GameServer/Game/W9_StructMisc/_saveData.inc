// ORIG T 0x08643642：等级低于 TowerOfDespairMgr 最小等级时直接返回 true；
// 否则经 StreamPool 取流组包：<<0x278、<<UID、GetInBuffer 填 0x18 记录、
// MsgQueueMgr::put(Q2)。异常安全由局部 CStreamGuard 析构承担。
// [b28d-23 N·N93] ⇒ N。ORIG 0x08643642
bool TOD_UserState::_saveData(CUser* pUser)
{
    TowerOfDespairMgr* mgr =
        static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);
    // [P3-#331 2026-09-11] P0/P183 删多余具名局部：ORIG 0x08643642 无 level 槽
    //（`mov %eax,%ebx` 直接保留调用结果于 callee-saved），我方 `const int level`
    // 多一槽（-0x18）并使帧 0x30 vs ORIG 0x20；比较改内联形态。
    if (pUser->get_charac_level() < static_cast<int>(mgr->GetMinLv()))
    {
        return true;
    }

    // [L8 2026-09-10 P259] ORIG 0x086436xx：Acquire 返回值直接经 %eax 传 ctor
    // （无 stream 具名局部）⇒ 我方 stream 槽（-0x14/-0x18）多 2 槽 + 4 条
    // load/store，并使帧 0x30 vs ORIG 0x20。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("TOD_UserState.cpp", 0x58), true);
    // ORIG：`(*guard)->operator<<(x)`（operator* de()）与 `guard->GetInBuffer`
    // （operator-> pt()）标识调用形态，勿改为直接成员调用。
    (*guard)->operator<<(0x278);
    (*guard)->operator<<(static_cast<int>(pUser->GetUID()));
    SIG_REQUEST_TOD_USER_STATE* sig =
        guard->GetInBuffer<SIG_REQUEST_TOD_USER_STATE>();
    // [P3-#331 2026-09-11] P183 删多余具名局部：ORIG 0x086436f3 直接
    // `cmpl $0x0,-0xc(%ebp); jne <then>`（无 setne/mov 的 bool 槽）；
    // 我方 `bool ok = (sig != 0)` 多 1 槽 + 3 条（setne/movb/cmpb）。
    if (sig == 0)
    {
        return false;
    }
    memset(sig, 0, sizeof(*sig));
    sig->m_characNo = pUser->get_charac_no(-1);
    // [P3-#331 2026-09-11] ORIG 0x0864374e 先存 +0xe（this+0x16=m_lastEnterLayer）
    // 再 0x08643762 存 +0xc（this+0x14=m_todayLayer）；原顺序相反多 1 处交换。
    sig->m_lastEnterLayer = m_data.m_lastEnterLayer;   // sig+0xe = this+0x16
    sig->m_todayLayer = m_data.m_todayLayer;           // sig+0xc = this+0x14
    sig->m_firstEnterTime = m_data.m_firstEnterTime;   // sig+4=+0xc
    sig->m_enterCount = m_data.m_enterCount;           // sig+8=+0x10
    sig->m_lastEnterTime = m_data.m_lastEnterTime;     // sig+0x10=+0x18
    sig->m_lastClearTime = m_data.m_lastClearTime;     // sig+0x14=+0x1c
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(Q2)
    return true;
}