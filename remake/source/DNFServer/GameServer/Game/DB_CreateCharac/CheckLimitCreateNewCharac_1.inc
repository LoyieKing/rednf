bool DB_CreateCharac::CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig,
                                                bool traceFlag)
{
    // ORIG 头部 `mov 0x10(%ebp),%eax; mov %al,-0x7c(%ebp)`：bool 形参规范化
    // spill（空 if 体被 DCE，仅留 spill 两条）。
    if (traceFlag)
    {
    }

    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    // P102：三次结果检查复用同一 bool 局部（ORIG 同一 -0x29 槽 + 三独立 break 岛）；
    // 声明在 db 之前使 4.4 给 bool 更深槽（-0x2c 尾 -0x29），db 落 -0x28。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    localtime_r(&nowSec, &localTime);
    localTime.tm_hour = 0;
    localTime.tm_min = 0;
    localTime.tm_sec = 0;
    nowSec = mktime(&localTime);   // ORIG mktime 结果写回 -0x20 同槽

    // ORIG 初始化序 -0x44 先/-0x48 后 ⇒ lastAccessTime 先声明（4.4 后声明深槽）。
    unsigned int lastAccessTime = 0;
    unsigned int count = 0;
    db->set_query(
        "seLect count, unix_timestamp(last_access_time) from limit_create_character where m_id=%s",
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
    // P232：ORIG 多一个 -0x20 死槽（`movl $0x0,-0x20` 后无读取）⇒ 补 dead 局部
    // 使后续标量槽（nRows/db/nowSec/trace）整体深 4 对齐。
    int deadSlot = 0;
    (void)deadSlot;
    for (int i = 0; i < nRows; ++i)
    {
        ok = db->fetch();
        if (!ok)
        {
            break;
        }
        ok = db->get_uint(0, count);
        if (!ok)
        {
            break;
        }
        ok = db->get_uint(1, lastAccessTime);
        if (!ok)
        {
            break;
        }
        if (count > 1)
        {
            sig->m_resultCode = 5;
            // P227 全表达式临时链：ORIG ctor 前右起求值并以 %esi/%edi/%ebx 保活。
            cMyTrace(__PRETTY_FUNCTION__, 0xf25, 0)(
                "[LIMIT NEW CHARAC MID] %s,%d,%d,%d",
                NumberToString(sig->m_accId, 0),
                sig->m_characNo, lastAccessTime,
                G_CEnvironment()->get_server_group());
            return false;
        }
    }
    return true;
