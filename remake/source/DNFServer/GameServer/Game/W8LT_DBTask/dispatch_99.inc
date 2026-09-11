// ============================================================================
// DB_UpdateQueryCountTimerLogdb::dispatch —— ORIG 0x0841c458（报告全文）
// ============================================================================
// ORIG 0x841c458：GetDBHandle(DB_HANDLE_4,0) → GetQueryCounter() →
// GetQueryCountTable()（map<int,QUERY_INFO>，QUERY_INFO{count,int;
// responseTime,double}）→ s_systemTime_.getCurSec() → for 遍历：
//   qid=first、total=second.count；total>0 时：
//   respTime=(int)(1000.0 * second.responseTime)（_DAT_08c5dec8=1000.0，
//   fldcw 0x0c + fistpl 截断，即普通 (int) 强转）→
//   set_query("inSert into log_query_stat(occ_time,q_id,total,response_time,
//   gc_no) values(from_unixtime(%d),%d,%d,%d,%d)", curSec, qid, total,
//   respTime, G_CEnvironment()->get_channel_no()) → exec(true)（返回值
//   丢弃：ORIG xor $1/test/jmp 死代码）；end() 每轮调用（P13 反例）。
// 循环结束 counter->Reset() → return 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0841c458
// [r3] 已修：`if (!db->exec(true)) goto L_next_query;` → `continue;` ⇒ 复现
//   ORIG 0841c56d `xor $0x1,%eax; test %al,%al; jmp <incr>` 死岛 ⇒
//   ndiff 2→0、gap 2→0（101/101、差集空）。旧 P168a③「封顶」判断作废：
//   4.4 对 `continue;`（循环尾）保留物化，对 `goto L_next_query;` 才 jump-thread。
bool DB_UpdateQueryCountTimerLogdb::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    CQueryCounter* counter = db->GetQueryCounter();
    std::map<int, CQueryCounter::QUERY_INFO>& table =
        counter->GetQueryCountTable();
    int curSec = GlobalData::s_systemTime_.getCurSec();
    for (std::map<int, CQueryCounter::QUERY_INFO>::iterator it =
             table.begin();
         it != table.end(); it++)
    {
        int qid = it->first;
        int total = it->second.count;
        // P223 负条件 goto 共享尾空标签（复现 4.1 if-join 岛：jle→nop + 体尾 jmp）。
        if (0 >= total)
            goto L_next_query;
        {
            int respTime = (int)(1000.0 * it->second.responseTime);
            db->set_query(
                "inSert into log_query_stat(occ_time,q_id,total,response_time,gc_no) values(from_unixtime(%d),%d,%d,%d,%d)",
                curSec, qid, total, respTime,
                G_CEnvironment()->get_channel_no());
            // [b28c-33 复核 102/100：残 2 = jmp+nop 岛] ORIG 0x841c56d-0x841c572
            // `xor $1;test %al;jmp <incr>` 死岛 =
            // 4.1 对 `if(!exec()) continue;` 保留物化 + 块尾 jmp（jle 落点
            // nop pad 在前）；4.4 threading 消 jmp ⇒ N75 残留一行。
            if (!db->exec(true))
                continue;
        }
    L_next_query:;
    }
    counter->Reset();
    return 1;
}