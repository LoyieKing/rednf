bool DB_ForbidUserToPlay::dispatch(int userIdx, int slot, Stream* stream)
{
    DB_ForbidUserToPlayView* sig = reinterpret_cast<DB_ForbidUserToPlayView*>(
        stream->GetOutBuffer<SIG_FORBID_USER_TO_PLAY>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    int characNo = 0;
    bool flag = false;
    int days = 0;
    characNo = sig->m_characNo;
    flag = sig->m_flag;
    days = sig->m_days;
    // ORIG 0x8421b55-0x8421b64 双 goto 钉块序（技巧23+24）：
    // `movzbl -0xd,%eax; xor $0x1,%eax; test %al,%al; jne b66(1-arg块)` +
    // `cmpl $0,-0xc; jne b82(2-arg块)`；4.4 对 `if(a&&b){}else{}` 直判
    // cmpb$1 融合且块序反（2-arg fall）。fall-through 块 = 1-arg SQL。
    // bool 槽 + `!flag`（f1 探针形）⇒ `movzbl slot,%eax; xor $1; test
    // %al; jne` 物化；uchar 槽一切写法折 cmpb/test。
    // V2（FULL 内联 if + 单 stub）实测劣化：消掉 1 行 stub jmp 但反出
    // cond2 方向（je↔jne）+ 双 body 填参各 +2 行装载序翻转 ⇒ 净差 4 行
    // >1。维持 V1（双 goto 钉块序，残留 1 行 = 4.4 label-stub bb 的
    // fall-through jmp，N-JMP 族）。
    // [b28d-solo-W8LT_DBTask] 去 else 花括号（裸 if 链）：4.4 不再为 else 块出口
    // 发 fall-through jmp，SIMPLE_SQL 直接跟随条件跳转（ORIG 形）。
    if (!flag)
        goto SIMPLE_SQL;
    if (days != 0)
        goto FULL_SQL;
SIMPLE_SQL:
    db->set_query(
        "upDate charac_stat set forbidden_to_play=0, forbidden_due_to= 0 where charac_no = %d",
        characNo);
    goto AFTER_SQL;
FULL_SQL:
    db->set_query(
        "upDate charac_stat set forbidden_to_play=1, forbidden_due_to = date_add(now(), interval %d day) where charac_no = %d",
        days, characNo);
AFTER_SQL:;
    if (!db->exec(true))
        return 0;
    return 1;
