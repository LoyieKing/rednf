// ---------------------------------------------------------------------------
// DB_SwapCreatureItem::makeRequest —— ORIG 0x0841fef2（报告
// docs/class_func_reports/DB_SwapCreatureItem/makeRequest.md 全文已读）：
// Acquire("DBThread.cpp",0x5662) → CStreamGuard(stream,true)
//   → guard << 0x8c(int) → guard << -1(int)
//   → sig = guard.GetInBuffer<SIG_SWAP_CREATURE_ITEM>()
//   → m_empty1=0、m_slot2=param_2、m_itemIdx1=param_1、m_empty2=1、
//     m_slot1=0、m_itemIdx2=0（逐字段独立重读 sig，O0 无 CSE）
//   → MsgQueueMgr::put(DB_Q(2), guard)（析构时入队）。
void DB_SwapCreatureItem::makeRequest(int itemIdx1, int slot2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5662), true);
    guard.operator*()->operator<<((int)0x8c);
    guard.operator*()->operator<<((int)-1);
    SIG_SWAP_CREATURE_ITEM* sig =
        guard->GetInBuffer<SIG_SWAP_CREATURE_ITEM>();
    sig->m_empty1 = 0;
    sig->m_slot2 = slot2;
    sig->m_itemIdx1 = itemIdx1;
    sig->m_empty2 = 1;
    sig->m_slot1 = 0;
    sig->m_itemIdx2 = 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}