bool Arad_DB_kSetCharacCreateTime::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081853b2：upDate charac_info set create_time=from_unixtime(%u) where charac_no=%u。
    //   %ebx 缓存返回（catch 0x08185544 / 成功 0x08185565 / merge 0x0818556a）。
    register bool ret;
    arad::SigSetCharacCreateTime* sig = stream->GetOutBuffer<arad::SigSetCharacCreateTime>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x773, "SigSetCharacCreateTime is null.");
        ret = false;
        goto done;   // ORIG 0x08185346：`mov$0,%ebx; jmp merge`（共享 merge，勿复制）
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                " upDate charac_info set create_time = from_unixtime(%u)  where charac_no = %u ",
                sig->m_createTime, sig->m_characNo))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x788, "[SetCharacCreateTime] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
