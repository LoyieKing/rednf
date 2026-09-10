bool Arad_DB_kSaveRewardEventItem::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081858de：inSert into event_reward_item_arad values (%d,%u,%u,%u,from_unixtime(%u),now())。
    //   %ebx 缓存返回（4.1 EH 形）。
    register bool ret;
    arad::SigSaveRewardEventItem* sig = stream->GetOutBuffer<arad::SigSaveRewardEventItem>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7cc, "SigRewardEventItem is null.");
        ret = false;
        goto done;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                " inSert into event_reward_item_arad values (%d, %u, %u, %u, from_unixtime(%u), now())",
                sig->m_field00, sig->m_field04, sig->m_field08,
                sig->m_field10, sig->m_field0c))   // ORIG 0x08185985：第5实参 = +0x0c dword 直存
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e2)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x7e1, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e2);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
