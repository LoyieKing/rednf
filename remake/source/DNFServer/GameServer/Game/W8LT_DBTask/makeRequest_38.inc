void DB_SaveAccountCargo::makeRequest(int userIdx, unsigned int accId,
                                      CAccountCargo* cargo)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c9f), true);
    guard.operator*()->operator<<((int)0x1f1);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)accId);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast；ORIG RHS-first）。
    SIG_ACCOUNT_CARGO_DATA* sig =
        guard->GetInBuffer<SIG_ACCOUNT_CARGO_DATA>();
    memset(sig, 0, 0xd60);
    sig->m_capacity = cargo->m_capacity;
    sig->m_money = cargo->m_money;
    memcpy(sig->m_cargo, &cargo->m_slots[0], 0xd58);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    cargo->SetStable();
