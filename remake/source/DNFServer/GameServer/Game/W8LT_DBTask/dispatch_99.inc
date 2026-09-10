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
                goto L_next_query;
        }
    L_next_query:;
    }
    counter->Reset();
    return 1;
