void DB_UpdateCeraShopPurchaseCount::makeRequest(unsigned int accId,
                                                 unsigned long itemUid,
                                                 int count, int param4)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbbd6), true);
    guard.operator*()->operator<<((int)0x2d9);
    guard.operator*()->operator<<(param4);
    SIG_CERASHOP_PURCHASE_COUNT* sig =
        guard->GetInBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (sig == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbbde, 5)(
            "[PURCAHSE BONUS] ptrData is null");
        return;  // ORIG jmp → ~CStreamGuard 后 ret（无入队）
    }

    sig->m_count = count;
    sig->m_id = accId;
    sig->m_itemUid = (unsigned int)itemUid;   // ORIG +0x4 仅 4 字节写
    sig->m_purchaseCnt = 0;

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
