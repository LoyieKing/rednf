// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
bool DB_CheckCharacName::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_CHECK_CHARAC_NAME* sig = stream->GetOutBuffer<SIG_CHECK_CHARAC_NAME>();
    unsigned int mId = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect m_id from charac_info where charac_name='%s' limit 1",
        reinterpret_cast<DB_CheckCharacNameView*>(sig)->m_charName);
    unsigned char flag = 0;
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() != 0)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_int(0, mId))
            return 0;
        if (reinterpret_cast<DB_CheckCharacNameView*>(sig)->m_mId == mId)
            flag = 1;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4ec), true);
    guard.operator*()->operator<<(0x2a3);
    guard.operator*()->operator<<(slot);
    // ORIG 0x84432e7-0x84432fd 尾两 store：`mov -0xc,%eax; movzbl -0xd,%edx;
    // mov %dl,(%eax)` / `mov -0x18,%eax; movzbl 0x22(%eax),%edx; mov -0xc,%eax;
    // mov %dl,0x1(%eax)`（RHS 先载）。4.4 中 LHS 表达式带 reinterpret_cast 恒
    // 翻转为 LHS 地址先载（探针 /tmp/w8lt_ord3.o::f5），LHS 用 view 类型直存
    // 则保 RHS 先（探针 /tmp/w8lt_ord4.o::f6）⇒ 单 view 指针直存。
    DB_ExceptionMidView* out =
        reinterpret_cast<DB_ExceptionMidView*>(
            guard.operator->()->GetInBuffer<SIG_GOLD_CONTROL_EXCEPTION_MID>());
    out->m_flag = flag;
    out->m_char =
        reinterpret_cast<DB_CheckCharacNameView*>(sig)->m_byte22;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}