bool Arad_DB_kAvatarHiddenOptionSave::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x081860f4：upDate user_items set hidden_option where charac_no/slot/it_id。
    //   %ebx 缓存返回（4.1 EH 形）。
    register bool ret;
    arad::SigAradAvatarHiddenOptionSave* sig =
        stream->GetOutBuffer<arad::SigAradAvatarHiddenOptionSave>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x854, "SigAradAvatarHiddenOptionSave is null.");
        ret = false;
        goto done;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query(
                "upDate user_items set hidden_option=%d where charac_no=%d and slot=%d and it_id=%d",
                (int)sig->m_hiddenOption, sig->m_characNo, (int)sig->m_slot, sig->m_itId))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x868, "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
}