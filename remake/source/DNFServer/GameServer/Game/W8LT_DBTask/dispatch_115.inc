bool DB_CheckSecuReward::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_CHECK_USER_SECU_REWARD* sig =
        stream->GetOutBuffer<SIG_CHECK_USER_SECU_REWARD>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    unsigned int lastCheck = 0;
    db->set_query(
        "seLect unix_timestamp(last_check_time) from member_security_grade where m_id=%s",
        NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
    // P168a 五步自检定论（goto 落点岛/正判双分支/短路合判三形实跑均劣化
    // ——首段 sete 岛+3 jmp；4.1 `xor$1;test` 物化段与尾 v 双常数岛为
    // 跨版本 bool 物化顶③b 族）⇒ 保留 && 直判原形，标 N。S 类
    // missing-calls:ptEv 已修（guard-> 形）。
    // [b28d-solo-W8LT_DBTask] ORIG 0x08444262-0x08444296：负极性短路物化
    // （`call exec; xor $1; test %al; jne FAIL` / `n_rows==0 je FAIL` /
    // `call fetch; xor $1; test; je PASS` + FAIL/PASS 双常数岛）⇒ 原码是
    // `if (!A || B==0 || !C) return 1;` 早返形（正极性 && 包裹反产无 xor 直判）。
    if (!db->exec(true) || db->get_n_rows() == 0 || !db->fetch())
        return 1;
    {
        db->get_uint(0, lastCheck);
        time_t nowT;
        time(&nowT);
        int mod = (int)nowT % 0x15180;
        nowT = nowT - mod - 0x2a30;
        if (mod > 0x1274f)
            nowT = nowT + 0x15180;
        if ((int)lastCheck < (int)nowT)
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xab25),
                true);
            guard.operator*()->operator<<(userIdx);
            guard.operator*()->operator<<(slot);
            // ORIG 0x8444397：`lea -0x28; call CStreamGuard::operator->
            // (ptEv); call GetInBuffer<SIG_SEND_USER_REWARD>` = `guard->`
            // 形（台账 S 类 missing-calls:ptEv 修复；`operator*()->` 形会
            // 丢 ptEv call）。
            // ORIG 0x84444xx store：`mov -0x18,%eax; mov 0x4(%eax),%edx;
            // mov -0xc,%eax; mov %edx,(%eax)` —— RHS 先载（sig+4 单级字段
            // 解引用）+ LHS reload；4.4 中 LHS 带 cast 翻为地址先载 + RHS
            // `add $4` 三级（探针 /tmp/w8lt_ord3.o::f5）⇒ LHS 取 view 型
            // 变量直存、RHS 取 view 字段。
            DB_SecuRewardOutView* out =
                reinterpret_cast<DB_SecuRewardOutView*>(
                    guard.operator->()->GetInBuffer<SIG_SEND_USER_REWARD>());
            out->m_reward =
                reinterpret_cast<DB_SecuRewardSigView*>(sig)->m_param4;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
    }
    return 1;
