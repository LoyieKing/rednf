// ============================================================================
// DB_UpgradeAccountCargo::dispatch —— ORIG T 0x0843af76。
// StreamPool::Acquire("DBThread.cpp",0x8bfb) → CStreamGuard → <<userIdx <<slot →
// GetInBuffer<SIG_UPGRADE_ACCOUNT_CARGO_RESULT>（memset 0x8）→ >>u32 a >>u32 b →
// GetDBHandle(DB_HANDLE_2,0) → UPDATE account_cargo；exec 失败返 0；
// affected==0 → flag=0x15 发包(Q1)；否则 flag=0、capacity=b 发包(Q1)。
// ============================================================================
bool DB_UpgradeAccountCargo::dispatch(int userIdx, int slot, Stream* stream)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8bfb), true);
    (*guard)->operator<<(userIdx);
    (*guard)->operator<<(slot);
    SIG_UPGRADE_ACCOUNT_CARGO_RESULT* result =
        guard->GetInBuffer<SIG_UPGRADE_ACCOUNT_CARGO_RESULT>();
    memset(result, 0, sizeof(SIG_UPGRADE_ACCOUNT_CARGO_RESULT));
    unsigned int a = 0;
    unsigned int b = 0;
    *stream >> a;
    *stream >> b;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("upDate account_cargo set capacity = %d where m_id = %s",
                  b, NumberToString(a, 0));
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        result->m_flag = 0x15;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    result->m_flag = 0;
    result->m_capacity = b;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}