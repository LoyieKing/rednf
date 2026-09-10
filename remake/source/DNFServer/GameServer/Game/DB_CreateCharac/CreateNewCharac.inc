bool DB_CreateCharac::CreateNewCharac(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);

    db->set_query("seLect count(*) from charac_info where m_id=%s and delete_flag=0",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }
    int liveCount = 0;
    if (!db->fetch() || !db->get_int(0, liveCount) || liveCount >= 0x24)
    {
        sig->m_resultCode = 2;
        return false;
    }

    // G_CEnvironment()->m_dbConnInfo[DB_HANDLE_2].m_db —— ORIG map::operator[]
    // 于 env+0x68、STDBConnInfo m_db 在 +0x42；[b28d-Main·检查点18] 改成员直读
    //（原 get_db_conn_info() accessor 会发射 only-NEW 弱符号）。
    // [P183/P329 去缓存·ORIG 直证] ORIG 为 `movl $0x2,-0x24(%ebp)` + `call
    // G_CEnvironment; lea 0x68(%eax),%edx` + `call operator[]` + `add $0x42,%eax`
    // 全内联链式，**无** connMap/connInfo 具名缓存（我方原多两处 -0x10/-0xc 槽往返）。
    // 仅 connIdx 是 ORIG 实有的具名局部（-0x24 槽）。
    ENUM_DB_HANDLE_IDX connIdx = DB_HANDLE_2;       // 栈上局部初始值 2
    if (!_checkCharacName(db, G_CEnvironment()->m_dbConnInfo[connIdx].m_db, sig))
    {
        return false;
    }

    sig->m_pad534d[0] = 0;                          // byte +0x534d

    // serverGroup==10 && now<0x478e7051 的临时通道：追加老号数量检查，
    // 若该账号从无角色则记 m_pad534d[0]=1。
    if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 10 &&
        GlobalData::s_systemTime_.getCurSec() < (unsigned int)0x478e7051)
    {
        db->set_query("seLect count(*) from charac_info where m_id=%s",
                      NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            sig->m_resultCode = 2;
            return false;
        }
        int totalCount = 0;
        if (!db->fetch() || !db->get_int(0, totalCount))
        {
            sig->m_resultCode = 2;
            return false;
        }
        if (totalCount == 0)
        {
            sig->m_pad534d[0] = 1;
        }
    }

    // P214③/P312：ORIG 内联 `rep stos`（15 dword）清零 ⇒ 用 = {0} 形。
    char escapedName[60] = {0};
    db->escape_string(escapedName, sig->m_name);

    if ((signed char)sig->m_field23 >= 1)
    {
        if (!_checkCreateEventCharacter(sig))
        {
            return false;
        }
        _updateCreateEventCharacter(sig);
    }
    else if ((signed char)sig->m_field23 < 0)
    {
        sig->m_resultCode = 4;
        return false;
    }

    MakeInsertCharacInfo(db, sig, escapedName);
    // [P0 直证] ORIG index202 段为 `call <T>; mov %al,-0xd(%ebp); movzbl -0xd(%ebp),%eax;
    // xor $0x1,%eax`（具名 bool 局部槽往返 + 我方缺的 ext_movzbl）⇒ 补具名 ok。
    bool ok = db->exec(true);
    if (!ok)
    {
        // P227 全表达式临时链：ORIG NumberToString 于 ctor 前求值入 %ebx。
        cMyTrace(__PRETTY_FUNCTION__, 0x10e2, 5)(
            "DB_CreateCharac::CreateNewCharac, exec() ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        sig->m_resultCode = 1;
        return false;
    }
    return true;
