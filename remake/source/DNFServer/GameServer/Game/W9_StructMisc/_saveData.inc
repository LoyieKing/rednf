bool TOD_UserState::_saveData(CUser* pUser)
{
    const int level = pUser->get_charac_level();
    TowerOfDespairMgr* mgr =
        static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr);
    if (level < static_cast<int>(mgr->GetMinLv()))
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
    bool ok = (sig != 0);
    if (ok)
    {
        memset(sig, 0, sizeof(*sig));
        sig->m_characNo = pUser->get_charac_no(-1);
        sig->m_todayLayer = m_data.m_todayLayer;           // ORIG save 顺序：sig+0xc=+0x14
        sig->m_lastEnterLayer = m_data.m_lastEnterLayer;   // sig+0xe=+0x16
        sig->m_firstEnterTime = m_data.m_firstEnterTime;   // sig+4=+0xc
        sig->m_enterCount = m_data.m_enterCount;           // sig+8=+0x10
        sig->m_lastEnterTime = m_data.m_lastEnterTime;     // sig+0x10=+0x18
        sig->m_lastClearTime = m_data.m_lastClearTime;     // sig+0x14=+0x1c
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(Q2)
    }
    return ok;
