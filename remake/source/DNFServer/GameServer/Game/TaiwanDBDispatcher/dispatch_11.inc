// ORIG 0x0815b010 T LoadGeolocation::dispatch（报告 LoadGeolocation/dispatch.md）：
// GetDBHandle(1,0) null→throw "handle null"；get_server_group → set_query
// geo_allow_country → exec 失败 throw "exec"；逐行 get_str(0,buf,0x10) 失败 throw
// "get_str" → SigLoadGeolocation::set；count>0x2b 时 sendCountryData+init；末尾
// sendCountryData 一次。catch(const char*) logFormat 0x13c 返 0，成功返 1。
bool LoadGeolocation::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        int serverGroup = G_CEnvironment()->get_server_group();
        db->set_query(
            "seLect country_code from geo_allow_country where server_group = %d",
            serverGroup);
        if (!db->exec(true))
            throw "exec";
        // P214③/P202①：ORIG 0x0815b0e5 区为 4 条独立 `movl $0,-N(%ebp)`
        // （16B 逐 dword），memset 生 call；逐 dword 视图赋值复现。
        union GeoCodeBuf { char c[0x10]; unsigned int d[4]; };
        GeoCodeBuf buf;
        buf.d[0] = 0;
        buf.d[1] = 0;
        buf.d[2] = 0;
        buf.d[3] = 0;
        Taiwan::SigLoadGeolocation sig;
        sig.init();
        while (db->fetch())
        {
            // ORIG 循环内是 call memset@plt（仅循环前的预清零为 4 movl）。
            memset(buf.c, 0, 0x10);
            if (db->get_str(0, buf.c, 0x10) != 1)
                throw "get_str";
            sig.set(buf.c);
            if (sig.m_count > 0x2b)
            {
                sendCountryData(sig);
                sig.init();
            }
        }
        sendCountryData(sig);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x13c,
            "[Taiwan, GeoIP] select geo_allow_country. (error:%s)", e);
        return 0;
    }
}
