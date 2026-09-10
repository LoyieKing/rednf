bool Arad_DB_kAvatarEnduranceSave::dispatch(int code, int uid, Stream* stream)
{
    (void)code;
    (void)uid;
    // %ebx 缓存返回（4.1 EH 形，ORIG 0x081862d0 同族）。
    register bool ret;
    arad::SigAradAvatarEnduranceSave* sig =
        stream->GetOutBuffer<arad::SigAradAvatarEnduranceSave>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x876,
                              "SigAradAvatarEnduranceSave is null.");
        ret = false;
        goto done;
    }
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";

        // ORIG set_query(h, fmt, +0x04=emblem_endurance, +0x00=ui_id)
        if (!db->set_query("upDate user_items set emblem_endurance=%d where ui_id=%d",
                           sig->endurance, sig->uiId))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x88a,
                              "[SigAradAvatarEnduranceSave] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
done:
    return ret;
