bool DB_CreateCharac::CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig)
{
    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    // P102：fetch/get_uint 两次检查复用同一 bool 局部（ORIG 同一 -0x19 槽）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    localtime_r(&nowSec, &localTime);
    localTime.tm_hour = 0;
    localTime.tm_min = 0;
    localTime.tm_sec = 0;
    // ORIG mktime 结果写回 -0x20 同槽（nowSec 复用，无独立 dayStart 槽）。
    nowSec = mktime(&localTime);

    unsigned int createTime = 0;
    db->set_query(
        "seLect unix_timestamp(create_time) from limit_create_character where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }

    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }
    int todayCount = 0;
    for (int i = 0; i < nRows; ++i)
    {
        ok = db->fetch();
        if (!ok)
        {
            break;
        }
        ok = db->get_uint(0, createTime);
        if (!ok)
        {
            break;
        }
        // P225 推广：拆嵌套 if 消 4.4 的 && bool 物化（ORIG 两跳同一目标）；
        // 比较写成 nowSec<=createTime（unsigned long 语义）复现 ORIG ja 无符号跳。
        if (nowSec <= createTime)
        {
            ++todayCount;
            if (todayCount > 1)
            {
                sig->m_resultCode = 5;
                return false;
            }
        }
    }
    return true;
