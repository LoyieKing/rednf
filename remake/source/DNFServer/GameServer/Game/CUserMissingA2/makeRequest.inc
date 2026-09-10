void DB_LoadCharacHotKeyOption::makeRequest(int uid, unsigned int accId,
                                            int hotkeyType)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x749a), true);
    guard.operator*()->operator<<(800);   // SIG_LOAD_CHARAC_HOTKEY_OPTION opcode
    guard.operator*()->operator<<(uid);

    struct View
    {
        unsigned int m_accId;             // +0x00（dispatch 作 SQL where 键）
        int m_hotkeyType;                 // +0x04
    };
    View* v = reinterpret_cast<View*>(
        guard.operator->()->GetInBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION>());
    v->m_accId = accId;
    v->m_hotkeyType = hotkeyType;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
