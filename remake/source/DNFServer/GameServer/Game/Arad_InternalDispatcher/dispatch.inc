bool Arad_DB_kAradEventItemLog::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081851dc：inSert into log_arad_event_stat (...) values (now(), %u, %u, %u, %d)。
    //   返回值经 %ebx 缓存（4.1 EH 形）：早退 0x08185224 mov$0,%ebx、
    //   成功 0x0818535e? mov$1,%ebx、catch 0x08185343 mov$0,%ebx。
    register bool ret;
    arad::SigAradEventItemLog* sig = stream->GetOutBuffer<arad::SigAradEventItemLog>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x643, "EVENT ITEM LOG, SigAradEventItemLog is null.");
        ret = false;
        goto done;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                " inSert into log_arad_event_stat ( occ_time, m_id, charac_no, item_id, item_cnt )  values "
                " ( now(), %u, %u, %u, %d ) ",
                sig->m_id, sig->m_characNo, sig->m_itemId, sig->m_itemCount))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x661,
                              "[EVENT_LOG] DB_DSP Error, insert log to log_arad_event_stat. (error:%s)",
                              e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
