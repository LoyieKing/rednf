bool DB_FindFactoryHub::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_FIND_FACTORY_HUB* sig = stream->GetOutBuffer<SIG_FIND_FACTORY_HUB>();
    unsigned int mId = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    char escName[0x3b] = { '\0' };
    db->escape_string(escName, sig->m_characName);
    db->set_query(
        "seLect m_id from charac_info where charac_name='%s' limit 1",
        escName);
    if (!db->exec(true))
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_int(0, mId))
        return 0;
    if (sig->m_mId != mId)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8253), true);
        **guard << 0x18a;
        // ORIG 0x8436d41：`mov 0x10(%ebp),%edx` = 形参 slot。
        **guard << slot;
        SIG_FIND_FACTORY_HUB_FATIGUE_CHECK* out =
            guard->GetInBuffer<SIG_FIND_FACTORY_HUB_FATIGUE_CHECK>();
        out->m_flag = sig->m_field21;
        out->m_mId = mId;
        strncpy(out->m_characName, sig->m_characName, 0x1d);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 1;
