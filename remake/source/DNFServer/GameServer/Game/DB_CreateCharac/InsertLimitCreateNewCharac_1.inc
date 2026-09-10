bool DB_CreateCharac::InsertLimitCreateNewCharac(SIG_CREATE_CHARAC* sig,
                                                 bool traceFlag)
{
    // ORIG 头部 `mov 0x10(%ebp),%eax; mov %al,-0x16c(%ebp)`：bool 形参规范化
    // spill（空 if 体被 DCE，仅留 spill 两条；P232 死局部占槽律）。
    if (traceFlag)
    {
    }

    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate limit_create_character set count=count+1, last_access_time=now() where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }
    if (db->getAffectedRowCount() == 0ULL)
    {
        db->set_query(
            "inSert into limit_create_character(m_id, count, last_access_time) values(%s, 1, now() )",
            NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            sig->m_resultCode = 2;
            return false;
        }
    }

    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    struct tm* lt = localtime_r(&nowSec, &localTime);
    char dateBuf[256];
    sprintf(dateBuf, "%04d-%02d-%02d %02d:%02d:%02d",
            lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
            lt->tm_hour, lt->tm_min, lt->tm_sec);

    // P227 全表达式临时链（同 SIG* 重载）：ORIG ctor 前右起求值 + callee-saved 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0xf61, 0)(
        "[LIMIT NEW CHARAC] %s,%d,%s,%d",
        NumberToString(sig->m_accId, 0),
        sig->m_characNo, dateBuf,
        G_CEnvironment()->get_server_group());
    return true;
