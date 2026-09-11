// [b28d-62] 修：迭代器增量改后置 `it++`（P28 增量侧：ORIG 调 operator++(int)）。
//   ORIG 0x085ecee6
void CQueryCounter::UpdateQueryCounter()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1f1), true);
    **guard << 0x7d;
    **guard << -1;
    int curTime = time(0);
    **guard << curTime;
    **guard << m_totalQueryCount;
    // [b28d-62] P28 增量侧：ORIG 增量段调 operator++(int)（后置，带独立 sret
    //   临时槽）⇒ 源形必写 `it++`（`++it` 走 ppEv 免临时槽）。
    // [2026-09-11] P283：ORIG 0x85ecfb6 `begin()` 后 `sub $0x4; jmp <条件块>`
    // （4.1 的 for 底部条件布局 + nop 填充岛），4.4 恒条件置顶 ⇒ 用
    // goto 尾条件形复现。
    std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
    goto query_loop_cond;
query_loop_body:
    {
        int qid = it->first;
        int count = it->second.count;
        int response = (int)(it->second.responseTime * 1000.0);
        if (count > 0)
        {
            **guard << qid;
            **guard << count;
            **guard << response;
        }
    }
    it++;
query_loop_cond:
    if (it != m_queryInfoMap.end())
        goto query_loop_body;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    Reset();
}