// ============================================================================
// DB_CheckAddFriend —— ORIG dispatch 0x0844c6e0（报告 dispatch.md）。
// stream 空/GetOutBuffer<SIG_CHECK_ADD_FRIEND> 空返 0。请求 +0x04 字节标志
// 非 0：GetDBHandle(DB_HANDLE_2,0) → escape_string(+0x05 名字,0x3b 缓冲) →
// set_query(seLect m_id from charac_info where charac_name='%s' and
// delete_flag=0) → exec/fetch/get_n_rows==1/get_int(0) 全成后 isGM(m_id)：
// GM → 结果 0x5a，非 GM → 0；任一失败保持初始 1。标志==0 → 结果 0。
// 组包 Acquire("DBThread.cpp",0xc05e) → <<0x33e → <<slot →
// GetInBuffer<SIG_CHECK_ADD_FRIEND>：+0=结果、strncpy(+0x05,名字,0x1e)
// → put(DISPATCH_Q) 返 1；GetInBuffer 空返 0。
// ============================================================================
// [SweepW8LT b28d] DIFF：gap 18（our 200 / ORIG 218）封顶。p167 22/22 [0缺0多]
// （CStreamGuard C1/C2、D1/D2 同址别名）。残差：① P292 int→bool 归一化——ORIG
// escape_string 后 `test %eax,%eax; sete %al; test %al,%al; jne`，set_query/
// exec/fetch/get_int 后各多一条 `xor $1`（4.4 直接 test %al; je）；② P223 五个
// 失败出口各带 `nop; jmp` 中继岛（4.4 jump-thread 消岛，-10）；③ 两处
// `outCode = 0` 4.1 tail-merge 共享（844c8a9），4.4 保留两份（+1）。ORIG 0844c6e0
bool DB_CheckAddFriend::dispatch(int userIdx, int slot, Stream* stream)
{
    int result = 0;              // -0x2c 查询 m_id
    MySQL* db = 0;               // -0x28
    int outCode = 1;             // -0x24 回包结果码
    SIG_CHECK_ADD_FRIEND* req = 0;  // -0x20
    SIG_CHECK_ADD_FRIEND* out = 0;  // -0x1c
    char name[0x3b] = { '\0' };      // -0x6f escape 目标
    if (stream == 0)
        return 0;
    req = stream->GetOutBuffer<SIG_CHECK_ADD_FRIEND>();
    if (req == 0)
        return 0;
    if (req->m_notGmFlag != 0)
    {
        db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                               E_SERVER_GROUP_NONE);
        // P223：ORIG 0x0844c7c5/0x0844c7ee/0x0844c813/0x0844c863/0x0844c884
        // 五个失败出口各带独立 `nop; jmp` 中继岛（0x1d2/0x1d5/0x1d8/0x1db/0x1de），
        // 合并 && 短路链被 4.4 jump-thread 成单出口 ⇒ 改多源裸 goto 到同一尾标签。
        if (db == 0)
            goto send_packet;
        if (!db->escape_string(name, req->m_characName))
            goto send_packet;
        if (!db->set_query(
                "seLect m_id from charac_info where charac_name='%s' and delete_flag=0",
                name))
            goto send_packet;
        if (!(db->exec(true) && db->fetch() && db->get_n_rows() == 1))
            goto send_packet;
        if (!db->get_int(0, result))
            goto send_packet;
        // [A04-r5c] C05③/P223 同目标合并：ORIG 唯一一份清零块 @0x0844c8a9
        // （`movl $0x0,-0x24(%ebp); jmp +0x1df`），两条 jcc 同目标 0844c8a9
        // 实证（flag==0 的 `je`@0x0844c79b 与 isGM==false 的 `je`@0x0844c89e）；
        // GM 路径 0x0844c8a0 `movl $0x5a,-0x24; jmp <after>` 跳过清零块。
        // 原 `else outCode=0;` 双份写法使 4.4 发两份 movl $0（多 1 条）。
        if (GlobalData::s_pGMAccounts_->isGM((unsigned int)result))
        {
            outCode = 0x5a;
            goto send_packet;
        }
    }
    goto set_zero;
set_zero:
    outCode = 0;
send_packet:
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc05e), true);
    guard.operator*()->operator<<(0x33e);
    guard.operator*()->operator<<(slot);
    out = guard.operator->()
              ->GetInBuffer<SIG_CHECK_ADD_FRIEND>();
    if (out != 0)
    {
        out->m_result = outCode;
        strncpy(out->m_characName, req->m_characName, 0x1e);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    return 0;
}