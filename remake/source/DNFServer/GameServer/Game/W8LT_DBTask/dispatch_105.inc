bool DB_LoadDnfHackLog::dispatch(int userIdx, int slot, Stream* stream)
{
    // [b28c-27] AE：循环体 if 块按 P223 goto 尾标签岛复现 ORIG `ja <nop>` +
    // body 尾 `jmp <addl>; nop`（842245d/842245e 实证）；其余为 .o 重定位伪差。
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    // ORIG 槽序：out(-0x10) 高于 inSig(-0x14) 高于 mId(-0x18) ⇒ inSig 先声明。
    DB_DnfHackLogView* inSig;
    SIG_LOAD_DNF_HACK_LOG* outSig =
        stream->GetOutBuffer<SIG_LOAD_DNF_HACK_LOG>();
    mId = reinterpret_cast<DB_DnfHackLogView*>(outSig)->m_mId;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5a8e), true);
    guard.operator*()->operator<<(0x106);
    guard.operator*()->operator<<(slot);
    inSig = reinterpret_cast<DB_DnfHackLogView*>(
        guard.operator->()->GetInBuffer<SIG_LOAD_DNF_HACK_LOG>());
    memset(inSig, 0, 0x3cf4);
    inSig->m_mId = mId;
    db->set_query(
        "seLect hack_type,cnt from dnf_hack_log where m_id=%s and (hack_type>=%d and hack_type<%d) and occ_date=CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR)",
        NumberToString(mId, 0), 0x65, 0xfa1);
    if (!db->exec(true))
        return 0;
    for (unsigned int i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        unsigned int hackType = 0;
        unsigned int cnt = 0;
        if (!db->fetch())
            return 0;
        if (!db->get_uint(0, hackType))
            return 0;
        // P223：ORIG 842245d `ja <nop>` + body 尾 `jmp <addl>; nop`（4.1 空块
        // 占位岛）；4.4 对 if 恒 jump-thread，复现形 = goto 尾标签。
        if (hackType > 0xfa0)
            goto L_next;
        if (!db->get_uint(1, cnt))
            return 0;
        inSig->m_cnt[hackType - 0x65] = cnt;
L_next:;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
