// [b28d-62] N82：p167 CLEAR。ORIG 0x085ed18a
// [P8S1 2026-09-11 本波复核] 维持封顶：N238 memset 长度常量经 %edx 中转(ORIG 0x85ed3d1 段)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
bool CQueryCounter::LoadQueryTable()
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    int ret = 0;
    char query[0x6001];
    mysql->set_query("seLect q_id,query from log_query_ref");
    if (!mysql->exec(true))
    {
        return false;
    }
    int nRows = mysql->get_n_rows();
    int i = 0;    // P191 ORIG -0xc 双 init 形
    for (i = 0; i < nRows; i = i + 1)
    {
        int qid = 0;
        memset(query, 0, 0x6001);
        if (!mysql->fetch())
        {
            // [G5 2026-09-10 P227 试验] ORIG 0x085ed18a 段为 cMyTrace ctor 后直接
            // operator()（varargs 在 ctor 前右起求值并保活 callee-saved）⇒ 改全表达式
            // 临时链；具名两语句形使 4.4 把 ctor 提前、帧 0x…。
            cMyTrace(__PRETTY_FUNCTION__, 0x22f, 5)("LoadQueryTable, fatch ERROR");
            return false;
        }
        if (!mysql->get_int(0, qid))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x235, 5)("LoadQueryTable, get_int(0) ERROR");
            return false;
        }
        if (!mysql->get_str(1, query, 0x6000))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x23b, 5)("LoadQueryTable, get_str(1) ERROR");
            return false;
        }
        if (!RegisterQuery(query, qid, 0))
        {
            return false;
        }
    }
    return true;
}