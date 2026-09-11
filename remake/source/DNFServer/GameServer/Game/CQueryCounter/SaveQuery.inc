// [b28d-62] N82：p167 CLEAR。ORIG 0x085ed3a2
// [P8S1 2026-09-11 本波复核] 本波已修：本波已修：控制流极性 + get_int 负条件(gap 3→0，our 118→121=ORIG)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
bool CQueryCounter::SaveQuery(const char* query, int& qid)
// [P8S1 2026-09-11 控制流极性对齐] ORIG 0x085ed3a2 实证：exec#1 0x085ed440
//   `xor $0x1,%eax; test %al,%al; je 0x85ed594`（成功 → 跳到**末尾**
//   `GetIdentityFromDB` 段 0x085ed594），失败 fallthrough 进 select 路径；
//   内层 exec#2 0x085ed47b 同形 `je 0x85ed4c2`，失败 fallthrough 即
//   `cMyTrace 0x265`（0x085ed482，紧邻 je 之后）；fetch 0x085ed4eb、get_int
//   0x085ed549 均为「负条件 + 早退」。⇒ 源码形 = `if (!exec) { … } qid =
//   GetIdentityFromDB(); return true;`（旧形正条件 if (exec) 使 4.4 走
//   fallthrough 成功路径 ⇒ 缺 xor 且 trace 段块序反，N104）。
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    char escaped[0xc001];
    memset(escaped, 0, 0xc001);
    mysql->escape_string(escaped, query);
    mysql->set_query("inSert into log_query_ref(query, query_hash) values('%s',password('%s'))",
                     escaped, escaped);
    if (!mysql->exec(true))
    {
        mysql->set_query("seLect q_id from log_query_ref where query_hash=password('%s')",
                         escaped, escaped);
        if (!mysql->exec(true))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x265, 5);
            trace("SaveQuery, exec() ERROR");
            return false;
        }
        if (mysql->get_n_rows() == 0)
        {
            return false;
        }
        if (!mysql->fetch())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x271, 5);
            trace("SaveQuery, fetch() ERROR");
            return false;
        }
        // [P8S1 2026-09-11] ORIG 0x085ed549 `xor $0x1,%eax; test %al,%al;
        //   je 0x85ed58d`（0x85ed58d = `mov $0x1,%eax; jmp leave`）⇒ 源码为
        //   **负条件 + 尾随 return true**（旧形正条件无 xor）。
        if (!mysql->get_int(0, qid))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x277, 5);
            trace("SaveQuery, get_int(0) ERROR");
            return false;
        }
        return true;
    }
    qid = GetIdentityFromDB(mysql);
    return true;
}