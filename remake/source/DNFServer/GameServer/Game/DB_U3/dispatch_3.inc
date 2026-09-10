bool DB_AccountHack_CheckMyAccount::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_CHECK_MY_ACCOUNT_IN_DB* sig =
        stream->GetOutBuffer<SIG_CHECK_MY_ACCOUNT_IN_DB>();
    unsigned int m_id = 0;
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    char name[0x3b] = { '\0' };

    db->escape_string(name, sig->m_name);   // +0x08
    db->set_query(
        "seLect m_id from charac_info where charac_name='%s' limit 1", name);
    if (!db->exec(true))
        return false;
    if (!db->fetch())
        return false;
    if (!db->get_int(0, m_id))
        return false;

    if (reinterpret_cast<DBU3AccountHackIn*>(sig)->m_accId != m_id)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8833), true);
        guard.operator*()->operator<<(0x18c);          // SIG_CHECK_MY_ACCOUNT_OK
        guard.operator*()->operator<<(slot);

        DBU3AccountHackOut* out =
            reinterpret_cast<DBU3AccountHackOut*>(guard.operator->()->GetInBuffer<SIG_CHECK_MY_ACCOUNT_OK>());
        out->m_a = reinterpret_cast<DBU3AccountHackIn*>(sig)->m_a;          // +0x00
        out->m_money = reinterpret_cast<DBU3AccountHackIn*>(sig)->m_money;  // +0x25
        out->m_id = m_id;

        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return true;
