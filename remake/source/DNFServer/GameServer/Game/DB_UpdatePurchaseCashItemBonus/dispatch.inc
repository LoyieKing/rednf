bool DB_UpdatePurchaseCashItemBonus::dispatch(int /*userIdx*/, int slot,
                                               Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int mId = 0;
    *s >> mId;

    // P0：ORIG 无 dm/sm 槽（G_CDataManager()→get_event_script_mng→get_db_table_name
    // 全链 %eax 直传，0x0843e293..e2ba）；std::string 局部 = ORIG -0x18 槽。
    std::string tableStr(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x19a));

    db->set_query("upDate %s set purchase_cnt = purchase_cnt + 1 where m_id=%s",
                  toTString(tableStr), NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into %s(m_id, purchase_cnt) values(%s, %u)",
                      toTString(tableStr), NumberToString(mId, 0), 1);
        if (!db->exec(true))
            return 0;
    }
    db->set_query("seLect purchase_cnt from %s where m_id=%s",
                  toTString(tableStr), NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() == 0)
        return 0;
    if (!db->fetch())
        return 0;
    unsigned int purchaseCnt = 0;
    if (!db->get_uint(0, purchaseCnt))
        return 0;

    // P259：Acquire 返回值直传 guard ctor（ORIG 无 stream 具名局部）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9407), true);
    guard.operator*()->operator<<(0x23c);
    guard.operator*()->operator<<(slot);   // ORIG 0x843e4c0：第二个 lsEi 参数 = 0x10(%ebp) = slot
    // P103：ORIG 0x0843e4d5 `call CStreamGuard::ptEv` 后再调 GetInBuffer。
    SIG_PURCHASE_CASHITEM_BONUS* sig =
        guard.operator->()->GetInBuffer<SIG_PURCHASE_CASHITEM_BONUS>();
    sig->m_purchaseCnt = purchaseCnt;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
