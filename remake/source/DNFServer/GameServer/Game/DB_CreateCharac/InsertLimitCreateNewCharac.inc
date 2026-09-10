bool DB_CreateCharac::InsertLimitCreateNewCharac(SIG_CREATE_CHARAC* sig)
{
    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into limit_create_character(m_id, create_time) values(%s, now() )",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 5;
        return false;
    }

    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    struct tm* lt = localtime_r(&nowSec, &localTime);
    char dateBuf[256];
    sprintf(dateBuf, "%04d-%02d-%02d %02d:%02d:%02d",
            lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
            lt->tm_hour, lt->tm_min, lt->tm_sec);

    // P227 全表达式临时链：ORIG 在 ctor 前右起求值 varargs 并以 %esi/%edi/%ebx
    // 跨 ctor 保活（0840182b 段），具名对象形恒 ctor 后求值。
    cMyTrace(__PRETTY_FUNCTION__, 0xe76, 0)(
        "[LIMIT NEW CHARAC] %s,%d,%s,%d",
        NumberToString(sig->m_accId, 0),
        sig->m_characNo, dateBuf,
        G_CEnvironment()->get_server_group());
    return true;
