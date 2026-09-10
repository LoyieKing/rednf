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
