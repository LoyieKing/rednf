bool DB_UpdateCeraShopPurchaseCount::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_CERASHOP_PURCHASE_COUNT* sig =
        stream->GetOutBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (sig == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbbac, 5)(
            "[PURCAHSE BONUS] ptrData is null.");
        return false;
    }

    if (!_selectPurchaseCount(sig->m_id, sig->m_purchaseCnt))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbbb2, 5)(
            "[PURCAHSE BONUS] select query fail.");
        return false;
    }

    if (sig->m_purchaseCnt == 0)
    {
        sig->m_purchaseCnt = 1;
        _insertPurchaseCount(sig->m_id);
    }
    else
    {
        sig->m_purchaseCnt = sig->m_purchaseCnt + 1;
        _updatePurchaseCount(sig->m_id, sig->m_purchaseCnt);
    }

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbbc1), true);
    guard.operator*()->operator<<((int)0x2d9);   // ORIG operator*() → operator<<(int)
    guard.operator*()->operator<<(slot);         // ORIG 0x10(%ebp) 第 3 形参
    SIG_CERASHOP_PURCHASE_COUNT* replySig =
        guard->GetInBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
    if (replySig == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbbc9, 5)(
            "[PURCAHSE BONUS] ptrData is null");
        return false;
    }

    *replySig = *sig;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return true;
