bool Arad_DB_kResetLimitedCreateCharacPerMid::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08186492：truncate limit_create_character。
    // ORIG 4.1 EH 函数 bool 返回值经 %ebx 缓存、尾部 `mov %ebx,%eax` 出栈
    //   （成功 0x081865e8 `mov $0x1,%ebx`；catch 0x081865c7 `mov $0,%ebx`）。
    register bool ret;
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        if (!db->set_query("truncate limit_create_character"))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        // ORIG 0x08186492 布局：成功块 `mov$1,%ebx` 落在 cleanup 之后 fall 入
        //   merge（0x081865e8）⇒ 成功赋值须在 try-catch 之后（E 形，技巧23 系）。
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x8e1, "[ResetLimitedCreateCharacPerMid] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    ret = true;
    return ret;   // ORIG 0x081865ed 直存 `mov %ebx,%eax`（技巧22 同族：bool 绑 ebx 免归一）
