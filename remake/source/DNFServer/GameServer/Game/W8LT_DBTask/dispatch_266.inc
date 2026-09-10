bool DB_LoadCharacHotKeyOption::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_B,
                                                  E_SERVER_GROUP_NONE);
    SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION* req =
        stream->GetOutBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION>();
    db->set_query(
        "seLect key_option from member_key_option where m_id = %s and key_type = %d",
        NumberToString(req->m_accId, 0),
        req->m_hotkeyType);
    if (!db->exec(true))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x746e), true);
    guard.operator*()->operator<<(0x320);
    guard.operator*()->operator<<(slot);
    // [Lane31W8LTDBTaskView] 权威头具名直访问（+0x00 权威名 m_uid）。
    SIG_LOAD_CHARAC_HOTKEY_OPTION* out =
        guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC_HOTKEY_OPTION>();
    out->m_uid = req->m_accId;
    out->m_hotkeyType = (unsigned char)req->m_hotkeyType;
    if (db->get_n_rows() == 0)
    {
        // [Lane31W8LTDBTaskView] +0x05 为 CHotkeyOption 字节视角（C++98 匿名
        // union 不得含有构造函数成员）⇒ 取址转类型调真实 clear（保留 ORIG 的
        // _ZN14CHotkeyOption5clearEv 调用）。
        reinterpret_cast<CHotkeyOption*>(&out->m_hotkey)->clear();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    if (!db->fetch())
    {
        // ORIG 0x0842f513-0x0842f57b：cMyTrace(0x747f,5) + NumberToString。
        cMyTrace(__PRETTY_FUNCTION__, 0x747f, 5)(
            "DB_LoadCharacHotKeyOption::dispatch, fetch ERROR m_id=%s",
            NumberToString(req->m_accId, 0));
        return 0;
    }
    // ORIG 0x0842f586 `mov $0xed,%ebx` + 对齐剥离慢路（P312/P202①）。
    static const int compressedBufferSize = 0xed;  // ORIG _ZZ…DB_LoadCharacHotKeyOption…E20compressedBufferSize
    char buf[0xed] = { '\0' };
    if (!db->get_binary(0, buf, compressedBufferSize))
    {
        // ORIG 0x0842f60e-0x0842f664：cMyTrace(0x7489,5) + NumberToString。
        cMyTrace(__PRETTY_FUNCTION__, 0x7489, 5)(
            "DB_LoadCharacHotKeyOption, get_binary(0) ERROR m_id=%s",
            NumberToString(req->m_accId, 0));
        return 0;
    }
    int binLen = db->get_binary_length(0);
    unsigned int destLen = 0xbc;
    if (!uncompress_zip((char*)&out->m_hotkey, destLen, buf,
                        (unsigned int)binLen))
        return 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
