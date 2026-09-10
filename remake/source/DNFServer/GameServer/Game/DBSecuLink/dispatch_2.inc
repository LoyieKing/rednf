bool DB_SecurityCardActivate::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SECURITY_CARD_ACTIVATE* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_ACTIVATE>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate member_security_card set apply_flag=1, cert_flag=1, "
        "cert_cnt = 0 where m_id=%s and cert_key='%s'",
        NumberToString(sig->m_mId, 0), &sig->m_certKey[0]);
    db->exec(true);

    bool ok = false;   // -0x21（回写 +0x0a）
    if (db->getAffectedRowCount() == 1)
    {
        ok = true;
        turnOnSecurityFlag(db, sig->m_mId);
    }

    DB_CancelRestingUserRestrict::makeRequest(sig->m_mId, slot);

    if (sig->m_historyFlag != 0)
        saveSecurityCardHistory(db, sig->m_mId,
                                ENUM_SECURITYCARD_MOD_TYPE_ACTIVATE);
    else
        saveSecurityCardHistory(db, sig->m_mId, ENUM_SECURITYCARD_MOD_TYPE_NONE);

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                   E_SERVER_GROUP_NONE);
    if (ok)
    {
        db2->set_query(
            "upDate member_security_grade set security_card_reg=now() "
            "where m_id=%s",
            NumberToString(sig->m_mId, 0));
        if (!db2->exec(true) || db2->getAffectedRowCount() == 0)
        {
            db2->set_query(
                "inSert into member_security_grade(m_id, security_card_reg) "
                "values(%s, now())",
                NumberToString(sig->m_mId, 0));
            if (!db2->exec(true))
                return 0;
        }
        // ORIG @0x0842ba64：CheckSecuReward 仅在 ok 分支调用（ok==0 时
        // +0x13b `je 0842ba80` 直达回包段；docs/class_func_reports/
        // DB_SecurityCardActivate/dispatch.md 汇编实证）——原实现误置于块外。
        DB_CheckSecuReward::makeRequest(slot, sig->m_mId, 0);
    }

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e89), true);
    guard.operator*()->operator<<(0xff);
    guard.operator*()->operator<<(slot);
    SIG_SECURITY_CARD_ACTIVATE* reply =
        guard->GetInBuffer<SIG_SECURITY_CARD_ACTIVATE>();
    reply->m_activateResult = ok;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
