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
    for (std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
         it != m_queryInfoMap.end(); it++)
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
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    Reset();
