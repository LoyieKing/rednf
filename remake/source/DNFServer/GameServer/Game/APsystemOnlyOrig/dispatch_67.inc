// [SweepW8LT b28d] DIFF：gap 14（our 320 / ORIG 334）封顶。p167 55/57：
// EXTRA memset×2 —— ORIG 4.1 对 `memset(buf,0,0x62)` 内联为 `mov $0x18,%ecx;
// rep stos; mov %ax,(%edx); add $0x2`（8 条）、对 0x2dc 内联 `mov $0xb7,%ecx;
// rep stos`（6 条）；4.4 -O0 恒 call memset（各 5 条）。长度语义已逐处核对
// （0x62 / 0x2dc 与 ORIG rep stos 计数一致），余为 N82 槽差。ORIG 0842f7f4
bool DB_LoadGameOption::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_REQ_LOAD_GAME_OPTION* req =
        stream->GetOutBuffer<SIG_REQ_LOAD_GAME_OPTION>();
    db->set_query(
        "seLect option_1,option_3,shortcut_emoticon from %s where m_id = %s",
        get_member_game_option(), NumberToString(req->m_accId, 0));
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() == 0)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x74c2), true);
        guard.operator*()->operator<<(0x12f);
        guard.operator*()->operator<<(slot);
        // [b28d-solo] P103/P167：ORIG 调 CStreamGuard::operator->（ptEv）取
        // 缓冲；`operator*()->` 会丢 ptEv（p167 差集 -2 ptEv/+2 deEv）。
        SIG_LOAD_GAME_OPTION* out =
            guard.operator->()->GetInBuffer<SIG_LOAD_GAME_OPTION>();
        ((CETCOption*)out)->clear();
        out->m_quick[0].clear();
        out->m_quick[1].clear();
        out->m_emoticon.clear();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    // [SweepIntervalSet] ORIG 0x0842f9b0 对 fetch/get_binary(0)/get_binary(1) 三处
    // 均发 `mov %al,-0x29(%ebp); movzbl -0x29(%ebp),%eax; xor $0x1; test; je`
    // ⇒ 原码复用同一具名 bool 局部（P0 槽往返 / P102）；直接 `if(!call())` 少两段。
    bool bRet = db->fetch();
    if (!bRet)
    {
        // [SweepIntervalSet] P227 全表达式临时链：ORIG 0x0842f9d3 在 cMyTrace ctor
        // 之前求值 NumberToString 并以 %ebx 跨 ctor 保活（`mov %eax,%ebx` → ctor →
        // `mov %ebx,0x8(%esp)`）；具名 trace 会把求值推迟到 ctor 之后（少 2 条）。
        cMyTrace(__PRETTY_FUNCTION__, 0x74dd, 5)(
            "DB_LoadGameOption::dispatch, fetch ERROR m_id=%s",
            NumberToString(req->m_accId, 0));
        return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x74e1), true);
    guard.operator*()->operator<<(0x12f);
    guard.operator*()->operator<<(slot);
    // [b28d-solo] P103/P167：ORIG 用 operator->（ptEv）。
    SIG_LOAD_GAME_OPTION* out =
        guard.operator->()->GetInBuffer<SIG_LOAD_GAME_OPTION>();
    ((CETCOption*)out)->clear();
    out->m_quick[0].clear();
    out->m_quick[1].clear();
    out->m_emoticon.clear();
    {
        static const int compressedBufferSize = 0x62;  // ORIG _ZZ…DB_LoadGameOption…E20compressedBufferSize
        // [SweepIntervalSet] ORIG 0x0842fad1 把 0x62 清零内联为
        // `lea; mov $0; mov $0x18,%ecx; mov %edx,%edi; rep stos; mov %edi,%edx;
        //  mov %ax,(%edx); add $0x2,%edx`（4 对齐已知 ⇒ 无对齐剥离检查）；
        // 4.4 探针实测 aligned(4) 数组零初始化逐条同形（普通数组会多 7 条对齐检查，
        // memset 恒 call）。语义等价：清零 0x62 字节。
        char buf[0x62] __attribute__((aligned(4))) = { '\0' };
        bRet = db->get_binary(0, buf, compressedBufferSize);
        if (!bRet)
        {
            // P227：ORIG 0x0842fb35 同样先求值 NumberToString 存 %ebx 再 ctor。
            cMyTrace(__PRETTY_FUNCTION__, 0x74f9, 5)(
                "DB_LoadGameOption, get_binary(0) ERROR m_id=%s",
                NumberToString(req->m_accId, 0));
            return 0;
        }
        unsigned int len0 = db->get_binary_length(0);
        unsigned int cap0 = 0x48;
        if (!uncompress_zip((char*)out, cap0, buf, len0))
            return 0;
    }
    {
        static const int compressedBufferSize = 0x2dc;  // ORIG …E20compressedBufferSize_0
        // [SweepIntervalSet] ORIG 0x0842fbcc 内联 `mov $0xb7,%edx; …; rep stos`
        // （0x2dc = 0xb7 dwords，无尾字节）；aligned(4) 数组零初始化 4.4 逐条同形。
        char buf[0x2dc] __attribute__((aligned(4))) = { '\0' };
        bRet = db->get_binary(1, buf, compressedBufferSize);
        if (!bRet)
        {
            // P227：ORIG 0x0842fc18 把 req->m_accId 读入 %ebx 跨 ctor 保活。
            cMyTrace(__PRETTY_FUNCTION__, 0x7515, 5)(
                "DB_LoadGameOption, get_binary(1) ERROR m_id=%u", req->m_accId);
            return 0;
        }
        unsigned int len1 = db->get_binary_length(1);
        unsigned int cap1 = 600;
        if (!uncompress_zip((char*)&out->m_quick[0],
                            cap1, buf, len1))
            return 0;
    }
    if (!get_compressed_blob_data(
            db, 2, (char*)&out->m_emoticon, 0x210, 0x16))
        return 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}