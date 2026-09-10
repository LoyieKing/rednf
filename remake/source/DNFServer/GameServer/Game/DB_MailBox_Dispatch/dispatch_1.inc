bool DB_MailBox_Req_CheckReceiver::dispatch(int userIdx, int slot, Stream* stream)
{
    // [b28d-48] N82 封顶（NDIFF 218，271/266 条）：帧/槽平移 + 判定链块序。
    SIG_MAILBOX_CHECK_RECEIVER* sig = stream->GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    int characNo = 0;
    int lev = 0;
    int deleteFlag = 0;
    unsigned int mId = 0;

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, (ENUM_SERVER_GROUP)0);
    MySQL* db3 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, (ENUM_SERVER_GROUP)0);

    // P244：ORIG 0x0841c8f8 `movl $SQL,-0x24(%ebp)`（0x0841c980 回读后压 0x4(%esp)）
    // ⇒ 原码是具名 char* 局部；store 发射在 name 清零之前（ORIG 0x0841c8f8 早于
    // 0x0841c8ff 的清零段）。
    const char* query =
        "seLect charac_no,lev,delete_flag,m_id from charac_info where charac_name='%s'";
    char name[0x3b] = { '\0' };  // ORIG 0x3b 字节（2*0x1d+1）
    db2->escape_string(name, sig->m_receiverName);
    db2->set_query(query, name);
    if (!db2->exec(true))
        return 0;

    for (unsigned int i = 0; i < (unsigned int)db2->get_n_rows(); ++i)
    {
        if (!db2->fetch())
            return 0;
        if (!db2->get_int(0, characNo))
            return 0;
        if (!db2->get_int(1, lev))
            return 0;
        if (!db2->get_int(2, deleteFlag))
            return 0;
        if (!db2->get_uint(3, mId))
            return 0;
    }

    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x509a), true);
    guard.operator*()->operator<<(0x7e);
    guard.operator*()->operator<<(slot);

    SIG_MAILBOX_CHECK_RECEIVER* out = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    out->m_receiverCharacNo = (unsigned int)characNo;
    out->m_receiverLevel = (unsigned int)lev;
    out->m_receiverDeleteFlag = (unsigned int)deleteFlag;
    out->m_receiverAccId = mId;
    memcpy(out->m_payload, sig->m_payload, 0x3f88);

    // P329：NumberToString 内联进实参位。ORIG 0x0841cb70 先读 sender 入 %ebx 跨
    // NumberToString call 保活、0x0841cb84 才 call NumberToString（cdecl 右→左求值）；
    // 具名 mIdStr 局部会把 sender 压回栈槽（4.4 不升格）并多一条中转。
    if (db3->set_query(
            "seLect count(*) from charac_black_list where m_id = %s and charac_no = %d",
            NumberToString(mId, 0), sig->m_senderCharacNo))
    {
        int blackCnt = 0;
        out->m_resultFlag = 0;
        // P225：ORIG 四个独立 `test/je` 到同一出口（0x0841cbd1/cbe0/cbfe/cc05）⇒
        // 嵌套 if；`&&` 链在 4.4 会把整链物化成 bool 岛（mov$1/jmp/mov$0/test/je）。
        if (db3->exec(true))
        {
            if (db3->fetch())
            {
                if (db3->get_int(0, blackCnt))
                {
                    if (blackCnt > 0)
                        out->m_resultFlag = 0x4d;
                }
            }
        }
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
