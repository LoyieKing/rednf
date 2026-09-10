void CQueryCounter::IncreaseQueryCount(const std::string& query)
{
    if (query.find("log_query_ref", 0) != std::string::npos)
    {
        return;
    }
    if (query.find("from db_connect", 0) != std::string::npos)
    {
        return;
    }
    std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator it =
        m_queryIDMap.find(query);
    // [b28d-62] P220 极性翻转：ORIG 0x085eccfc 调 operator==（`it == end` 为真直落
    //   未命中分支），我方 `!=` 生相反块序；m_bLogDB 正极性（`test %al; je else`，
    //   ORIG 0x085ecd0f 实证）⇒ 未命中分支内先 stream 后 SaveQuery。
    if (it == m_queryIDMap.end())
    {
        if (m_bLogDB)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1d2), true);
            **guard << 0xbe;
            **guard << -1;
            **guard << (int)query.size();
            guard->put_binary((void*)query.c_str(), (int)query.size());
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
        else
        {
            int qid = 0;
            if (SaveQuery(query.c_str(), qid))
            {
                RegisterQuery(query.c_str(), qid, 0);
            }
        }
    }
    else
    {
        m_current = it->second;
        // [G5 2026-09-10 P167] ORIG 0x085ecc5e 调用差集：我方多 1×
        // `_ZNKSt17_Rb_tree_iteratorISt4pairIKiN13CQueryCounter10QUERY_INFOEEEptEv`。
        // 实测 `(*m_current).second` 形反劣（换出 +3 deEv 且 ORIG 需 ptEv×2）⇒ 维持
        // `->` 形；试后置自增（P38 反向：`= x+1` 单语句 SAVE_EXPR 合并成单次 ptEv，
        // 拆/`++` 形可出第二处访问）。
        if (m_current->second.count == 0)
        {
            m_totalQueryCount = m_totalQueryCount + 1;
        }
        m_current->second.count++;
    }
