void DB_LogBuyItem::makeRequest(memberIdentificationNumber_t accId,
                                unsigned int characNo,
                                unsigned int itemIdx, unsigned int type,
                                ENUM_DBLOG_ITEM_TYPE value)
{
    if (characNo == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                            "localchina/localchina_DBThread.cpp", 0x59b),
                        true);
    **guard << 0x251;
    **guard << -1;
    **guard << accId;
    **guard << characNo;
    **guard << itemIdx;
    **guard << type;
    **guard << (int)value;
    cMyTrace(__PRETTY_FUNCTION__, 0x5af, 0)(
        "DB_LogBuyItem::makeRequest m_id=%s nCharacNo=%d nItemID=%u nCount=%u nType=%d",
        NumberToString(accId, 0), characNo, itemIdx, type, value);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}