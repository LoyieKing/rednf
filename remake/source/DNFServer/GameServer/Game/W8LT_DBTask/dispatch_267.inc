bool DB_LoadEventLevelupSupport::dispatch(int userIdx, int slot, Stream* stream)
{
    int characNo = 0;
    *stream >> characNo;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc26d, 5)(
            "[ERROR] Database Handle is NULL [index:%d]", 2);
        return 0;
    }
    try
    {
        db->set_query(
            "seLect level, state from event_levelup_support where charac_no=%d",
            characNo);
        if (!db->exec(true))
            throw (unsigned int)0xc277;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc279), true);
        guard.operator*()->operator<<(0x350);
        guard.operator*()->operator<<(slot);
        // [Lane31W8LTDBTaskView] 权威头具名直访问。
        SIG_EVENT_LEVEUP_SUPPORT_INFO* out =
            guard->GetInBuffer<SIG_EVENT_LEVEUP_SUPPORT_INFO>();
        int rowCount = db->get_n_rows();
        out->m_count = rowCount;
        // ORIG 0x0844cebc：循环前 `movw $0,-0x4a/-0x4c` 两 ushort 槽预清
        // （P198①/P232：声明在循环外，get_ushort(int,unsigned short&) 引用形参
        // 物化），循环尾 movzwl 回写成员。
        unsigned short level = 0;
        unsigned short state = 0;
        for (int i = 0; i < rowCount; ++i)
        {
            if (!db->fetch())
                throw (unsigned int)0xc28c;
            if (!db->get_ushort(0, level))
                throw (unsigned int)0xc28d;
            if (!db->get_ushort(1, state))
                throw (unsigned int)0xc28e;
            out->m_entries[i].m_level = level;
            out->m_entries[i].m_state = state;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    catch (unsigned int e)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc297, 5)(
            "[ERROR] DB_LoadEventLevelupSupport. [line:%d]", e);
        return 0;
    }
