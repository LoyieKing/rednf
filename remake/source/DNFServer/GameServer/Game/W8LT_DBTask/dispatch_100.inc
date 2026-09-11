// ============================================================================
// DB_UpdateQueryCountTimerDb::dispatch —— ORIG 0x0841c5d4（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x841c5d4：GetDBHandle(DB_HANDLE_2,0) → GetQueryCounter() →
// CQueryCounter::UpdateQueryCounter()（迁移 m_queryInfoMap 计数
// m_totalQueryCount）→ return 1。
bool DB_UpdateQueryCountTimerDb::dispatch(int userIdx, int slot,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    CQueryCounter* counter = db->GetQueryCounter();
    counter->UpdateQueryCounter();
    return 1;
}