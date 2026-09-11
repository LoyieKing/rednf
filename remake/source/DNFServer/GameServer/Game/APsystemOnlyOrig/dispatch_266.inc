// ============================================================================
// DB_LoadCharacHotKeyOption::dispatch —— ORIG 0x0842f3a0
// GetDBHandle(DB_HANDLE_B,0) → GetOutBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_
// OPTION>(+0 accId u32 / +4 keyType int) → seLect key_option from
// member_key_option where m_id=%s and key_type=%d；exec 失败返 0。回包流
// 0x320/slot + SIG_LOAD_CHARAC_HOTKEY_OPTION（+0 accId、+4 keyType byte、
// +5 CHotkeyOption）。无行 → CHotkeyOption::clear(out+5) → put 返 1。
// 有行：fetch 失败返 0；get_binary(0, buf[0xed]) 失败返 0；
// get_binary_length → destLen=0xbc → uncompress_zip(out+5) 失败返 0；
// put(DISPATCH_Q) 返 1。
// ============================================================================
// [b28d-solo-3] DIFF：`char buf[0xed]={'\0'}`（P312）已消 p167 EXTRA memset
// （差集 CLEAR）；余 N82 槽差（ORIG db(-0x2c) 6 读 vs 我方 -0x24 2 读 + 帧 4B）。
// [r3 负结论] destLen/binLen 声明序互换（P331）实测**反劣**：ndiff 2→4、gap 0
//   不变（ORIG 0842f3a0 该块 dst(lea/add) 先、len `mov -0x1c(%ebp),%edx` 后 =
//   4.1 左→右求值；4.4 恒右→左）⇒ 回滚，判 N 封顶。
// ORIG 0842f3a0。
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
    // [H4 2026-09-11 D 组取证] ORIG 0x0842f600 `mov %al,-0x1d(%ebp)` +
    // 0x0842f603 `movzbl -0x1d(%ebp),%eax` + `xor $1; test; je` ⇒ 原码此处**有**
    // 具名 bool 局部（P0/P102 同局部失败出口）；我方内联 `!call()` 少 2 条且全帧浅 4B。
    // （同函数 exec/fetch/uncompress_zip 三处 ORIG 无槽：0x0842f428/0x0842f523/
    //   0x0842f6b1 均直 `xor $1;test;je`，勿一并加局部。）
    bool bRet = db->get_binary(0, buf, compressedBufferSize);
    if (!bRet)
    {
        // ORIG 0x0842f60e-0x0842f664：cMyTrace(0x7489,5) + NumberToString。
        cMyTrace(__PRETTY_FUNCTION__, 0x7489, 5)(
            "DB_LoadCharacHotKeyOption, get_binary(0) ERROR m_id=%s",
            NumberToString(req->m_accId, 0));
        return 0;
    }
    int binLen = db->get_binary_length(0);
    unsigned int destLen = 0xbc;
    if (!uncompress_zip((char*)&out->m_hotkey, destLen, buf, binLen))
        return 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}